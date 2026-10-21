# Todo CLI Manager — Manual do usuário

Gerenciador local de tarefas e subtarefas para terminal, implementado em
C++20. As tarefas principais são armazenadas no arquivo `Todofile`; as
configurações, os logs e o banco das subtarefas ficam no diretório `.todo/`.

> Execute os comandos dentro do diretório do projeto que contém o `Todofile`.
> Para descrições com espaços, use aspas.

## Requisitos

- Windows.
- GCC/G++ com suporte a C++20 e `make` (por exemplo, MinGW-w64).
- Python 3 e o launcher `py -3` para usar o backend JSON.

O código-fonte do SQLite já está incluído no projeto.

## Compilação

Na raiz do repositório:

```powershell
make run
```

O executável é gerado em `bin/todo.exe`. Adicione a pasta `bin` ao `PATH` ou
execute o binário pelo caminho completo. Para consultar os comandos:

```powershell
todo help
todo help --specify subtask
```

## Início rápido

```powershell
todo init
todo add "Implementar o parser"
todo add "Escrever testes" --done
todo list
todo switch 1
todo check
```

Exemplo de `Todofile`:

```text
1. [ ] Implementar o parser
2. [X] Escrever testes
```

## Comandos

| Comando | Descrição |
| --- | --- |
| `todo help` | Lista os comandos disponíveis. |
| `todo help --specify <comando>` | Mostra informações de um comando. |
| `todo version` | Mostra a versão do executável. |
| `todo init` | Cria o `Todofile` e o repositório local `.todo`. |
| `todo init --subtasks` (`-st`) | Inicializa o banco de subtarefas conforme a configuração. |
| `todo add "<descrição>" [--done]` (`-d`) | Adiciona uma tarefa, opcionalmente concluída. |
| `todo list [opções]` | Lista até 20 tarefas por padrão. |
| `todo remove <número>` | Remove uma tarefa e renumera as demais. |
| `todo switch <número>` | Alterna a tarefa entre concluída e pendente. |
| `todo edit <número> "<descrição>"` | Altera a descrição de uma tarefa. |
| `todo count [opção]` | Conta tarefas, subtarefas ou arquivos de log. |
| `todo config [opção valor]` | Exibe ou altera uma opção de configuração. |
| `todo check` | Valida as entradas do `Todofile` e a contagem armazenada. |
| `todo restore [opções]` | Repara arquivos do repositório e/ou banco de subtarefas. |
| `todo subtask <ação> ...` | Gerencia subtarefas. |

### Opções de listagem

| Opção | Alias | Descrição |
| --- | --- | --- |
| `--all` | `-a` | Lista todas as tarefas. |
| `--checked` | `-c` | Lista apenas tarefas concluídas. |
| `--unchecked` | `-un` | Lista apenas tarefas pendentes. |
| `--logs` | `-l` | Lista os caminhos dos arquivos de log diários. |

```powershell
todo list --all
todo list --checked
todo list --unchecked
todo list --logs
```

### Opções de contagem

| Opção | Alias | Descrição |
| --- | --- | --- |
| `--checked` | `-c` | Conta tarefas concluídas. |
| `--unchecked` | `-un` | Conta tarefas pendentes. |
| `--logs` | `-l` | Conta arquivos de log. |
| `--subtasks` | `-st` | Conta subtarefas no backend selecionado. |

```powershell
todo count
todo count --checked
todo count --subtasks
```

## Subtarefas

Cada subtarefa tem um ID próprio e pertence a uma tarefa principal. Na criação,
o programa solicita a data de vencimento e a prioridade:

```powershell
todo subtask add 1 "Revisar tratamento de erros"
```

Use `--done` (`-d`) para criar a subtarefa como concluída:

```powershell
todo subtask add 1 "Confirmar a publicação" --done
```

Comandos de consulta:

```powershell
todo subtask list
todo subtask list --all 1
todo subtask list --id 3
todo subtask list --priority high
todo subtask list --status pending
```

Comandos de alteração:

```powershell
todo subtask edit 3 "Descrição atualizada"
todo subtask remove 3
```

Durante a edição, o programa pergunta a descrição, a tarefa associada, a
prioridade, o status e a data de vencimento. Responda `--` para manter o valor
atual. Valores aceitos:

- Prioridades: `none`, `low`, `medium`, `high`, `urgent`.
- Status: `pending`, `in_progress`, `completed`, `on_hold`, `cancelled`.

## Configuração e armazenamento

Consulte a configuração atual:

```powershell
todo config
```

Altere uma opção por vez:

```powershell
todo config nosql true
todo config readable true
todo config checkable true
```

Valores booleanos aceitos: `true`/`false`, `on`/`off` e `1`/`0`.

| Opção | Padrão | Efeito atual |
| --- | --- | --- |
| `nosql` | `false` | Seleciona JSON quando `true`; SQLite quando `false`. |
| `readable` | `true` | É armazenada na configuração; não muda o formato das tarefas principais. |
| `checkable` | `true` | É armazenada na configuração; não muda o formato das tarefas principais. |

### Tarefas principais

As tarefas principais são sempre texto simples no `Todofile`, com numeração
sequencial e estado:

```text
1. [ ] Preparar as notas da versão
2. [X] Executar testes de regressão
```

### Subtarefas em SQLite

SQLite é o backend padrão. O arquivo `.todo/subtasks.db` contém a tabela
`subtask`, com identificador, ID da tarefa associada, descrição, prioridade,
data de criação, data de vencimento opcional, data de conclusão e status.

Prioridade e status são armazenados como inteiros:

| Valor | Prioridade | Status |
| ---: | --- | --- |
| 0 | `none` | `pending` |
| 1 | `low` | `in_progress` |
| 2 | `medium` | `completed` |
| 3 | `high` | `on_hold` |
| 4 | `urgent` | `cancelled` |

### Subtarefas em JSON (NoSQL)

O backend JSON salva uma lista de objetos em `.todo/subtasks.json`. Cada
registro usa campos de texto para prioridade e status:

```json
[
  {
    "id": 1,
    "task_id": 1,
    "subtask": "Revisar tratamento de erros",
    "priority": "high",
    "date_impl": "2026-10-21",
    "due_date": "2026-10-28",
    "date_compl": "",
    "status": "pending"
  }
]
```

Para inicializar o repositório usando JSON:

```powershell
todo init
todo config nosql true
todo init --subtasks
```

O backend JSON depende do script `commands/command/subtasks/subtasks.py`,
invocado com `py -3`.

## Verificação e restauração

`todo check` valida a sintaxe e a numeração sequencial das tarefas e verifica
se o contador em `.todo/config/count.bin` corresponde ao `Todofile`.

`todo restore` repara o repositório, recalcula o contador, restaura as opções
padrão e cria ou migra o banco de subtarefas. O padrão seleciona SQLite.

```powershell
todo restore
todo restore --repo
todo restore --count
todo restore --sub
todo restore --config
```

Aliases: `--repo` (`-r`), `--count` (`-c`), `--sub` (`-s`) e
`--config` (`-cf`). `--config` redefine a configuração, incluindo
`nosql=false`. Para recriar ou migrar apenas o banco conforme a configuração
atual, use `todo restore --sub`.

Quando o banco selecionado não existe e o banco do outro formato existe,
`restore --sub` migra os registros para o formato selecionado. Se ambos já
existirem, não combina nem remove os registros do banco não selecionado.

## Arquivos do repositório

| Caminho | Conteúdo |
| --- | --- |
| `Todofile` | Lista textual das tarefas principais. |
| `.todo/config/config.bin` | Opções de configuração. |
| `.todo/config/count.bin` | Contagem armazenada das tarefas principais. |
| `.todo/logs/` | Logs diários de operações. |
| `.todo/subtasks.db` | Banco SQLite de subtarefas. |
| `.todo/subtasks.json` | Banco JSON de subtarefas. |

No Windows, a pasta `.todo` é marcada como oculta ao inicializar o repositório.

## Limitações conhecidas

- A implementação atual depende de APIs do Windows; Linux e macOS não são
  suportados pelo código atual.
- O backend JSON requer Python 3 e o launcher `py -3`.
- `readable` e `checkable` são persistidos, mas ainda não modificam a
  representação das tarefas principais.
- Prioridade e data de vencimento existem para subtarefas, não para tarefas
  principais.
- O repositório não contém atualmente um arquivo `LICENSE`.

## Referência de estrutura

A organização deste manual e do README foi inspirada no
[Best-README-Template](https://github.com/othneildrew/Best-README-Template),
publicado sob a [Unlicense](https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt).
O conteúdo foi escrito para refletir os comandos e o comportamento deste
projeto.
