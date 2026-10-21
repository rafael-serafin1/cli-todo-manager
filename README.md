# Todo CLI Manager

Gerencie tarefas e subtarefas diretamente pelo terminal com um executável leve, escrito em C++20. Os dados das tarefas principais ficam em um `Todofile` no diretório de trabalho; subtarefas, configurações e logs são mantidos localmente em `.todo/`.

[![C++](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Build](https://img.shields.io/badge/build-Makefile-427819)](#instalação)
[![Plataformas](https://img.shields.io/badge/plataformas-Windows%20%7C%20Linux%20%7C%20macOS-2ea44f)](#requisitos)

## Índice

- [Recursos](#recursos)
- [Requisitos](#requisitos)
- [Instalação](#instalação)
- [Primeiros passos](#primeiros-passos)
- [Comandos](#comandos)
- [Subtarefas](#subtarefas)
- [Arquivos e persistência](#arquivos-e-persistência)
- [Limitações conhecidas](#limitações-conhecidas)
- [Contribuição](#contribuição)
- [Licença](#licença)
- [Créditos](#créditos)

## Recursos

- Criar, listar, editar, concluir, reabrir e remover tarefas.
- Filtrar tarefas concluídas ou pendentes e contar tarefas e subtarefas.
- Organizar subtarefas com prioridade, status e data de vencimento.
- Conferir a integridade do `Todofile` e restaurar partes do repositório local.
- Consultar logs diários das operações.
- Manter os dados localmente, sem depender de serviços externos.

## Requisitos

- Compilador GCC/G++ com suporte a C++20.
- `make`.
- No Windows, uma distribuição MinGW-w64 compatível com `g++` e `-municode`.

O SQLite usado para persistir subtarefas está incluído no repositório (`include/sqlite3.c` e `include/sqlite3.h`); não é necessário instalar a biblioteca separadamente.

## Instalação

Clone o repositório e compile a partir da pasta do projeto:

```bash
git clone https://github.com/rafael-serafin1/cli-todo-manager.git
cd cli-todo-manager
make run
```

O alvo `make run` compila o SQLite incluído e gera o executável em `bin/todo` (ou `bin/todo.exe` no Windows). Para consultar os comandos usando o executável recém-compilado:

```bash
# Linux/macOS
./bin/todo help

# Windows PowerShell
.\bin\todo.exe help
```

Para executar `todo` sem informar o caminho completo, adicione a pasta `bin` ao `PATH` ou copie o executável para um diretório que já esteja nele.

## Primeiros passos

Execute `todo init` no diretório em que deseja manter suas tarefas. O comando cria o repositório local e o `Todofile`:

```bash
todo init
```

This creates the required repository and configuration files.

After that you can configure how Todo behaves.

---

# Commands

## Show help

```bash
todo help
```

Displays every available command and flag.

---

## Check repository integrity

```bash
todo check
```

Verifies that the task counter matches the tasks in `Todofile` and that task
entries are valid. This command does not modify repository files.

---

## Restore the Todofile repository

```bash
todo restore
```

Repairs the repository directories and configuration files, recalculates the
task count from `Todofile`, resets configuration to its defaults, and creates
or repairs the configured subtask database. Existing task and subtask data is
preserved.

Restore an individual part with `--repo` (`-r`), `--count` (`-c`), `--sub`
(`-s`), or `--config` (`-cf`).

---

## Configure Todo

```bash
todo config [OPTIONS]
```

Short flags

```bash
todo config -r -v -c
```

Long flags

```bash
todo config --readable --visible --checkable
```

### Available options

| Option | Description |
|---------|-------------|
| `--readable` | Stores tasks as plain text instead of binary |
| `--visible` | Stores the Todofile in the project root instead of `.todo/` |
| `--checkable` | Enables checkbox support for tasks |

Default configuration

```
Readable  : true
Visible   : true
Checkable : true
```

---

## Add a task

```bash
todo add "Finish README"
```

Example

```bash
todo add "Study operating systems"
```

---

## Remove a task

```bash
todo remove "3"
```

Removes the task with the specified index.

---

## Edit a subtask

```bash
todo subtask edit <SUBTASK_ID>
```

The command prompts for the subtask description, parent task ID, priority,
status, and due date. Enter `--` at any prompt to keep the current value.

---

## List tasks

```bash
todo list
todo switch 1
todo count
```

O `list` mostra até 20 tarefas por padrão; use `todo list --all` para exibir todas. `switch` alterna entre pendente e concluída. Execute `todo help` a qualquer momento para listar os comandos reconhecidos.

> **Dica:** coloque descrições com espaços entre aspas. Os comandos devem ser executados no diretório que contém o `Todofile` do projeto.

## Comandos

| Comando | Descrição |
| --- | --- |
| `todo help` | Mostra os comandos disponíveis. |
| `todo version` | Exibe a versão do executável. |
| `todo init` | Inicializa o repositório local de tarefas no diretório atual. |
| `todo add "<descrição>" [--done]` | Adiciona uma tarefa; `--done` (ou `-d`) cria a tarefa como concluída. |
| `todo list [opções]` | Lista até 20 tarefas por padrão. |
| `todo remove <número>` | Remove uma tarefa pelo número exibido na lista. |
| `todo switch <número>` | Alterna o status da tarefa entre concluída e pendente. |
| `todo edit <número> "<nova descrição>"` | Edita a descrição de uma tarefa. |
| `todo count [opção]` | Conta tarefas ou consulta contagens filtradas. |
| `todo config [opção valor]` | Sem argumentos, exibe a configuração; com argumentos, grava uma opção. |
| `todo check` | Confere a validade das entradas do `Todofile` e a consistência da contagem. |
| `todo restore [opções]` | Restaura a contagem, a estrutura do repositório e/ou o banco de subtarefas. |
| `todo subtask <ação> ...` | Cria, lista, edita ou remove subtarefas. Consulte [Subtarefas](#subtarefas). |

### Opções de `list`

| Opção | Alias | Efeito |
| --- | --- | --- |
| `--all` | `-a` | Lista todas as tarefas. |
| `--checked` | `-c` | Lista apenas as concluídas. |
| `--unchecked` | `-un` | Lista apenas as pendentes. |
| `--logs` | `-l` | Exibe os arquivos de log disponíveis. |

Exemplos:

```bash
todo list --all
todo list --checked
todo list --unchecked
todo list --logs
```

### Opções de `count`

| Opção | Alias | Efeito |
| --- | --- | --- |
| `--checked` | `-c` | Conta tarefas concluídas. |
| `--unchecked` | `-un` | Conta tarefas pendentes. |
| `--logs` | `-l` | Conta arquivos de log. |
| `--subtasks` | `-st` | Conta subtarefas. |

```bash
todo count
todo count --checked
todo count --subtasks
```

### Configuração e restauração

`todo config` sem argumentos mostra os valores atuais. Para gravar uma opção, use:

```bash
todo config readable true
todo config checkable false
```

As opções aceitam `true`/`false` e também `on`/`off` ou `1`/`0`.

`todo restore` sem opções tenta restaurar a contagem, a estrutura do repositório e o banco de subtarefas. Para selecionar partes específicas, use `--count` (`-c`), `--repo` (`-r`) e `--sub` (`-s`):

```bash
todo restore --count
todo restore --repo
todo restore --sub
```

## Subtarefas

Subtarefas pertencem a uma tarefa principal e têm identificador próprio, prioridade, status e data de vencimento. Ao criar uma subtarefa, o programa solicita interativamente a data, a prioridade e o status.

```bash
todo subtask add <número-da-tarefa> "<descrição>"
```

Prioridades aceitas: `none`, `low`, `medium`, `high` e `urgent`. Status aceitos: `pending`, `in_progress`, `completed`, `on_hold` e `cancelled`. Use `--done` (`-d`) para criar a subtarefa com status concluído.

```bash
todo subtask list
todo subtask list --all <número-da-tarefa>
todo subtask list --id <id-da-subtarefa>
todo subtask list --priority high
todo subtask list --status pending
todo subtask edit <id-da-subtarefa> "<nova descrição>"
todo subtask remove <id-da-subtarefa>
```

Ao editar, o programa solicita os demais campos. Informe `--` em uma solicitação para manter o valor existente.

## Arquivos e persistência

Os dados são locais ao diretório em que `todo init` foi executado:

| Caminho | Conteúdo |
| --- | --- |
| `Todofile` | Tarefas principais em texto simples, com número e status. |
| `.todo/config/config.bin` | Valores de configuração. |
| `.todo/config/count.bin` | Contagem armazenada das tarefas. |
| `.todo/logs/` | Logs de operações, organizados em arquivos diários. |
| `.todo/subtasks.db` | Banco SQLite das subtarefas, criado quando necessário. |

No Windows, a pasta `.todo` é marcada como oculta durante a inicialização.

## Limitações conhecidas

- As opções `readable` e `checkable` são gravadas na configuração, mas atualmente não alteram a serialização das tarefas principais: o `Todofile` continua sendo texto simples e as tarefas são exibidas com caixa de status.
- Prioridade e data de vencimento são campos de subtarefas; não são atributos das tarefas principais.
- O projeto ainda não contém um arquivo `LICENSE`. Consulte a seção [Licença](#licença) antes de reutilizar ou distribuir o código.

## Contribuição

Contribuições são bem-vindas. Abra uma issue para relatar um problema ou sugerir uma melhoria; para enviar código, abra um pull request com uma descrição clara da alteração. Antes de enviar, compile o projeto com:

```bash
make run
```

## Licença

Não há um arquivo de licença no repositório no momento. Portanto, o projeto ainda não declara formalmente os termos para uso, modificação ou redistribuição.

## Créditos

A organização das seções foi inspirada no [Best-README-Template](https://github.com/othneildrew/Best-README-Template), disponibilizado sob a [Unlicense](https://github.com/othneildrew/Best-README-Template/blob/master/LICENSE.txt). O conteúdo foi reescrito e adaptado às funcionalidades deste projeto.
