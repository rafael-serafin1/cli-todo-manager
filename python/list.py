import sys
import os

def main():
    try:
        with open('list.todo', 'r', encoding="utf-8") as f:
            list = f.readlines()
            print("========== TAREFAS ==========")
            for item in list:
                if item != '\n':
                    print("[ ] - " + item, end="")
            print("==============X==============")
    except Exception as e:
        print("Erro ao remover tarefa: {}".format(e))
        return 1
    return 0

if __name__ == "__main__":
    value = main()
    if value != 0:
        sys.exit(value)
    else:
        sys.exit(0)