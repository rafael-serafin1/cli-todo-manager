import sys 
import os

def main():
    if len(sys.argv) < 2:
        print("Erro: Nenhuma tarefa fornecida ao script Python.")
        sys.exit(1)
        
    todo = sys.argv[1]
        
    try:
        with open('list.todo', 'a', encoding="utf-8") as f:
            f.write(todo + '\n')
            print("Tarefa '{}' adicionada à lista de tarefas.".format(todo))
    except Exception as e:
        print("Erro ao adicionar tarefa: {}".format(e))
        sys.exit(1)
    sys.exit(0)

if __name__ == "__main__":
    main()