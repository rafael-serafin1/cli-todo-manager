import sys 
import os

def main():
    if len(sys.argv) < 2:
        print("Erro: Nenhuma tarefa fornecida ao script Python.")
        sys.exit(1)
    todo = sys.argv[1]
    try:
        with open('list.todo', 'r', encoding="utf-8") as f:
            lines = f.readlines()
            
        new_lines = [
            line for line in lines
            if line.rstrip("\n") != todo
        ]
            
        with open('list.todo', 'w', encoding="utf-8") as f:
            f.writelines(new_lines)
            print("Tarefa '{}' removida da lista de tarefas.".format(todo))
            
    except Exception as e:
        print("Erro ao remover tarefa: {}".format(e))
        sys.exit(1)
    sys.exit(0)

if __name__ == "__main__":
    main()  