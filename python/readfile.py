import sys
import os

def main():
    if len(sys.argv) < 2:
        print("Erro: Nenhum arquivo fornecido ao script Python.")
        sys.exit(1)
    filename = sys.argv[1]
    try:
        if not os.path.exists(filename):
            print("Erro: O arquivo '{}' nao existe.".format(filename))
            sys.exit(1)
        with open(filename, 'r') as f:
            conteudo = f.read()
            print("--- Conteudo de {} ---".format(filename))
            print(conteudo)
            print("-----------------------")
        sys.exit(0)

    except Exception as e:
        print("Erro ao processar o arquivo: {}".format(e))
        sys.exit(1)

if __name__ == "__main__":
    main()