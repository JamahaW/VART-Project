from __future__ import annotations

from typing import Callable
from typing import Iterable
from typing import Optional

from bytelang.bytecode.abc import ArgumentValueType
from bytelang.bytecode.abc import CodeInstruction
from bytelang.bytecode.abc import Directive
from bytelang.bytecode.abc import DirectiveArgument
from bytelang.bytecode.abc import ProgramData
from bytelang.bytecode.abc import Statement
from bytelang.bytecode.abc import StatementType
from bytelang.bytecode.abc import UniversalArgument
from bytelang.bytecode.abc import Variable
from bytelang.content.impl.environments import Environment
from bytelang.content.impl.environments import EnvironmentInstructionArgument
from bytelang.content.impl.environments import EnvironmentsRegistry
from bytelang.content.impl.primitives import PrimitiveType
from bytelang.content.impl.primitives import PrimitiveWriteType
from bytelang.content.impl.primitives import PrimitivesRegistry
from bytelang.core.handlers.errors import BasicErrorHandler
from tools.filters import Filter


class CodeGenerator:
    """Генератор промежуточного кода."""

    def __init__(self, error_handler: BasicErrorHandler, environments: EnvironmentsRegistry, primitives: PrimitivesRegistry) -> None:
        self.__err = error_handler.getChild(self.__class__.__name__)
        self.__environments = environments
        self.__primitives = primitives

        self.__env: Optional[Environment] = None
        self.__constants = dict[str, UniversalArgument]()
        self.__marks_address = dict[int, str]()
        self.__variables = dict[str, Variable]()

        self.__mark_offset_isolated: int = 0
        self.__variable_offset: Optional[int] = None

        __DIRECTIVE_ARG_ANY = DirectiveArgument("constant value or identifier", ArgumentValueType.ANY)

        self.__DIRECTIVES: dict[str, Directive] = {
            "env": Directive(self.__directiveSetEnvironment, (
                DirectiveArgument("environment name", ArgumentValueType.IDENTIFIER),)),
            "def": Directive(self.__directiveDeclareConstant, (
                DirectiveArgument("constant name", ArgumentValueType.IDENTIFIER), __DIRECTIVE_ARG_ANY)),
            "ptr": Directive(self.__directiveDeclarePointer, (
                DirectiveArgument("pointer identifier", ArgumentValueType.IDENTIFIER),
                DirectiveArgument("primitive type", ArgumentValueType.IDENTIFIER),
                __DIRECTIVE_ARG_ANY)),
        }

        self.__METHOD_BY_TYPE: dict[StatementType, Callable[[Statement], Optional[CodeInstruction]]] = {
            StatementType.DIRECTIVE_USE: self.__processDirective,
            StatementType.MARK_DECLARE: self.__processMark,
            StatementType.INSTRUCTION_CALL: self.__processInstruction}

    def __checkArgumentCount(self, statement: Statement, need: tuple) -> None:
        need_count = len(need)
        got_count = len(statement.arguments)

        if need_count == got_count:
            return

        self.__err.writeStatement(statement, f"Invalid arg count. Need {need_count} (got {got_count})")

    def __checkNameAvailable(self, statement: Statement, name: str) -> None:
        if self.__env is None:
            self.__err.writeStatement(statement, f"Невозможно проверить идентификатор {name}, окружение не выбрано")
            return

        if name in self.__constants.keys() or name in self.__env.instructions.keys():
            self.__err.writeStatement(statement, f"Идентификатор {name} уже используется")

    def __checkNameExist(self, statement: Statement, identifier: str) -> None:
        if identifier not in self.__constants.keys():
            self.__err.writeStatement(statement, f"Идентификатор {identifier} не определён")

    def __addConstant(self, statement: Statement, name: str, value: UniversalArgument) -> None:
        self.__err.begin()
        self.__checkNameAvailable(statement, name)

        if value.identifier is not None:
            self.__checkNameExist(statement, value.identifier)

        if self.__err.isFailed():
            return

        self.__constants[name] = value

    def __writeArgumentFromPrimitive(self, statement: Statement, argument: UniversalArgument, primitive: PrimitiveType) -> Optional[bytes]:
        if argument.identifier:
            self.__checkNameExist(statement, argument.identifier)

            if self.__err.isFailed():
                return

            return self.__writeArgumentFromPrimitive(statement, self.__constants[argument.identifier], primitive)

        v = argument.exponent if primitive.write_type == PrimitiveWriteType.EXPONENT else argument.integer

        try:
            return primitive.write(v)

        except Exception as e:
            self.__err.writeStatement(statement, f"Не удалось выполнить преобразование: {e}")

    def __writeArgumentFromInstructionArg(self, statement: Statement, i: int, u_arg: UniversalArgument, i_arg: EnvironmentInstructionArgument) -> Optional[bytes]:
        if i_arg.pointing_type:
            if (var := self.__variables.get(u_arg.identifier)) is None:
                self.__err.writeStatement(statement, f"Аргумент ({i}) Обращение по указателю ({i_arg}) с помощью сырого значения недопустимо")
                return

            if var.primitive.size < i_arg.pointing_type.size:
                self.__err.writeStatement(statement, f"Аргумент ({i}): Размер переменной {var} меньше размера указателя примитивного типа аргумента {i_arg}. Передача значения будет с ошибками")
                return

        return self.__writeArgumentFromPrimitive(statement, u_arg, i_arg.primitive_type)

    def __directiveSetEnvironment(self, statement: Statement) -> None:
        if self.__env is not None:
            self.__err.writeStatement(statement, "Окружение должно быть выбрано однократно")
            return

        env_name = statement.arguments[0].identifier

        try:
            self.__env = self.__environments.get(env_name)

        except Exception as e:
            self.__err.writeStatement(statement, f"Не удалось загрузить окружение {env_name}\n{e}")
            return

        self.__variable_offset = int(self.__env.profile.pointer_heap.size)

    def __directiveDeclareConstant(self, statement: Statement) -> None:
        name, value = statement.arguments
        self.__addConstant(statement, name.identifier, value)

    def __directiveDeclarePointer(self, statement: Statement) -> None:
        typename, name, init_value = statement.arguments
        name = name.identifier
        self.__err.begin()

        if (primitive := self.__primitives.get(typename.identifier)) is None:
            self.__err.writeStatement(statement, f"Unknown primitive type: {primitive}")
            return

        self.__checkNameAvailable(statement, name)

        if init_value.identifier:
            self.__checkNameExist(statement, init_value.identifier)

        if self.__variable_offset is None:
            self.__err.writeStatement(statement, "variable offset index undefined. Must select env")

        arg_value = self.__writeArgumentFromPrimitive(statement, init_value, primitive)

        if self.__err.isFailed():
            return

        self.__addConstant(statement, name, UniversalArgument.fromInteger(self.__variable_offset))

        self.__variables[name] = Variable(address=self.__variable_offset, identifier=name, primitive=primitive, value=arg_value)

        self.__variable_offset += primitive.size

    def __processDirective(self, statement: Statement) -> None:
        if (directive := self.__DIRECTIVES.get(statement.head)) is None:
            self.__err.writeStatement(statement, f"Unknown directive: {statement.head}")
            return

        self.__err.begin()
        self.__checkArgumentCount(statement, directive.arguments)

        if self.__err.isFailed():
            return

        self.__err.begin()

        for i, (d_arg, s_arg) in enumerate(zip(directive.arguments, statement.arguments)):
            d_arg: DirectiveArgument
            s_arg: UniversalArgument

            if s_arg.type not in d_arg.type:
                self.__err.writeStatement(statement, f"Incorrect Directive Argument at {i + 1} type: {s_arg.type}. expected: {d_arg.type} ({d_arg.name})")

        if not self.__err.isFailed():
            directive.handler(statement)

    def __getMarkOffset(self) -> int:
        return self.__variable_offset + self.__mark_offset_isolated

    def __processMark(self, statement: Statement) -> None:
        if self.__env is None:
            self.__err.writeStatement(statement, "Невозможно создать метку пока не выбрано окружение")
            return

        mark_offset = self.__getMarkOffset()
        self.__marks_address[mark_offset] = statement.head
        self.__addConstant(statement, statement.head, UniversalArgument.fromInteger(mark_offset))

    def __processInstruction(self, statement: Statement) -> Optional[CodeInstruction]:
        self.__err.begin()

        if self.__env is None:
            self.__err.writeStatement(statement, "no env (need) select env")
            return

        if (instruction := self.__env.instructions.get(statement.head)) is None:
            self.__err.writeStatement(statement, f"unknown instruction: {statement.head}")
            return

        self.__err.begin()
        self.__checkArgumentCount(statement, instruction.arguments)

        if self.__err.isFailed():
            return

        self.__err.begin()

        code_ins_args = tuple(self.__writeArgumentFromInstructionArg(statement, i + 1, s_arg, i_arg) for i, (i_arg, s_arg) in enumerate(zip(instruction.arguments, statement.arguments)))

        if self.__err.isFailed():
            return

        ret = CodeInstruction(instruction=instruction, arguments=code_ins_args, address=self.__getMarkOffset())
        self.__mark_offset_isolated += instruction.size
        return ret

    def run(self, statements: Iterable[Statement]) -> tuple[tuple[CodeInstruction, ...], Optional[ProgramData]]:
        return tuple(Filter.notNone(self.__METHOD_BY_TYPE[s.type](s) for s in statements)), self.getProgramData()

    # noinspection PyTypeChecker
    def getProgramData(self) -> Optional[ProgramData]:
        if self.__env is None:
            self.__err.write("must select env")
            return

        return ProgramData(environment=self.__env, start_address=self.__variable_offset, variables=tuple(self.__variables.values()), constants=self.__constants, marks=self.__marks_address)
