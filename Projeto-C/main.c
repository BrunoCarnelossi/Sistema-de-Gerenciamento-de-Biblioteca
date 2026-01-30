#include <stdio.h>
#include <string.h>

//ESTRUTURAS
struct livro {
    int codigo;
    char titulo[101];
    char autor[81];
    char editora[61];
    int ano; //ano de publicacao
    int exemplares; //numero de exemplares disponiveis
    int status; //1 = disponivel, 0 = indisponivel
};

typedef struct { //criando o tipo data
    int dia;
    int mes;
    int ano;
} data;

struct usuario {
    int matricula;
    char nome[101];
    char curso[51];
    char telefone[16];
    data data_cadastro; //estrutura que armazena a data de cadastro
};

struct emprestimo {
    int codigo; //codigo do emprestimo
    int matricula; //matricula do usuario vinculado
    int codigo_livro;
    data data_emprestimo; //estrutura que armazena a data do emprestimo
    data data_devolucao; //estrutura que armazena a data prevista para devolucao do livro (7 dias apos emprestar)
    int status; //1 = ativo, 0 = devolvido
};

//CONSTANTES
#define MAX_USUARIOS 200
#define MAX_LIVROS 200
#define MAX_EMPRESTIMOS 300

//VARIAVEIS E VETORES GLOBAIS
struct usuario usuarios[MAX_USUARIOS];
int total_usuarios = 0;

struct livro livros[MAX_LIVROS];
int total_livros = 0;

struct emprestimo emprestimos[MAX_EMPRESTIMOS];
int total_emprestimos = 0;


//PROTOTIPO DAS FUNCOES
int diasDoMes(int mes, int ano);
int dataValida(data d);
data calcularDataPrevista(data d);
void limparBuffer();

void cadastrarUsuario();
void carregarUsuarios();

void cadastrarLivro();
void carregarLivros();

void emprestarLivro();
void carregarEmprestimos();
void salvarLivros();
void devolverLivro();
void salvarEmprestimos();

void pesquisarLivros();
int buscarLivroCodigo(int codigo);
int buscarLivroTitulo(const char *titulo);
int buscarLivrosAutor(const char *autor, int indices[]);

void pesquisarUsuarios();
int buscarUsuarioMatricula(int matricula);
int buscarUsuarioNome(const char *nome);

int buscarEmprestimoCodigo(int codigo);
void listarEmprestimosAtivos();
void renovarEmprestimo();


//FUNCAO PRINCIPAL
int main() {
    //carregando dados de usuarios, livros e emprestimos
    carregarUsuarios();
    carregarLivros();
    carregarEmprestimos();

    int opcao = -1;

    while (opcao != 0) { //menu
        printf("\n--------------------------------------------- MENU PRINCIPAL ---------------------------------------------\n");
        printf("[1]Cadastrar usuario - [2]Pesquisar usuarios - [3]Cadastrar livro - [4]Pesquisar livro - [5]Emprestar livro\n");
        printf("[6]Listar emprestimo ativos - [7]Devolver livro - [8]Renovar emprestimo - [0]Sair\n");

        printf("Digite uma opcao: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 0:
                printf("Saindo...");
                break;
            case 1:
                cadastrarUsuario();
                break;
            case 2:
                pesquisarUsuarios();
                break;
            case 3:
                cadastrarLivro();
                break;
            case 4:
                pesquisarLivros();
                break;
            case 5:
                emprestarLivro();
                break;
            case 6:
                listarEmprestimosAtivos();
                break;
            case 7:
                devolverLivro();
                break;
            case 8:
                renovarEmprestimo();
                break;
            default:
                printf("Opcao invalida!\n");
                break;
        }

    }

    return 0;
}


//IMPLEMENTACAO DAS FUNCOES
int diasDoMes(int mes, int ano) { //funcao que retorna quantos dias tem o mes informado
    switch(mes) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if ((ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0) //ano bissexto
                return 29;
            else
                return 28;
        default:
            return 0; //caso de erro
    }
}

int dataValida(data d) { //funcao que retorna 0 para data invalida e 1 para data valida
    if (d.ano < 1)
        return 0;

    if (d.mes < 1 || d.mes > 12)
        return 0;

    int dias = diasDoMes(d.mes, d.ano);

    if (d.dia < 1 || d.dia > dias)
        return 0;

    return 1; //esta valida
}


data calcularDataPrevista(data d) { //funcao que retorna a data prevista para devolver o livro (7 dias apos emprestimo ou renovacao)
    data resultado = d;
    resultado.dia += 7;

    int dias_mes = diasDoMes(resultado.mes, resultado.ano);

    //se passar do fim do mes
    if (resultado.dia > dias_mes) {
        resultado.dia -= dias_mes;
        resultado.mes++;

        //se passar de dezembro
        if (resultado.mes > 12) {
            resultado.mes = 1;
            resultado.ano++;
        }
    }

    return resultado;
}

void limparBuffer() { //funcao para lidar com limpeza de buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cadastrarUsuario() { //funcao para cadastrar um usuario e salvar no arquivo usuarios.txt
    FILE *arquivo;
    struct usuario usuario = {0};
    int ok = 0; //variavel de controle para garantir que o usuario digite a data no formato correto

    printf("\n----- CADASTRO DE USUARIO ---------------------------------------------\n");

    printf("\nNome completo: ");
    fgets(usuario.nome, sizeof(usuario.nome), stdin);
    usuario.nome[strcspn(usuario.nome, "\n")] = '\0';

    printf("\nMatricula: ");
    scanf("%d", &usuario.matricula);
    limparBuffer();

    printf("\nCurso: ");
    fgets(usuario.curso, sizeof(usuario.curso), stdin);
    usuario.curso[strcspn(usuario.curso, "\n")] = '\0';
    
    printf("\nTelefone: ");
    fgets(usuario.telefone, sizeof(usuario.telefone), stdin);
    usuario.telefone[strcspn(usuario.telefone, "\n")] = '\0';
    
    while (!ok) { //enquanto a data nao estiver no formato correto, ela nao sera aceita
        printf("\nData de hoje (dd/mm/aaaa): ");

        if (scanf("%d/%d/%d", &usuario.data_cadastro.dia, &usuario.data_cadastro.mes, &usuario.data_cadastro.ano) == 3 && dataValida(usuario.data_cadastro)) {
            ok = 1;
            limparBuffer();
        }
        else {
            printf("Formato invalido! Use dd/mm/aaaa\n");
            limparBuffer();
        }
    }

    arquivo = fopen("usuarios.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo 'usuarios.txt'.\n");
        return;
    }

    fprintf(arquivo, "%s;%d;%s;%s;%02d;%02d;%04d\n", usuario.nome, usuario.matricula, usuario.curso, usuario.telefone, usuario.data_cadastro.dia, usuario.data_cadastro.mes, usuario.data_cadastro.ano);

    fclose(arquivo);
    printf("Usuario salvo com sucesso em usuarios.txt!\n");
    carregarUsuarios();
}

void carregarUsuarios() { //funcao para carregar os dados de usuarios salvos caso exista um arquivo usuarios.txt
    FILE *arquivo = fopen("usuarios.txt", "r");
    if (!arquivo) { //ainda nao existe
        total_usuarios = 0;
        return;
    }

    total_usuarios = 0;
    struct usuario u;

    while (fscanf(arquivo, "%100[^;];%d;%50[^;];%15[^;];%d;%d;%d\n", u.nome, &u.matricula, u.curso, u.telefone, &u.data_cadastro.dia, &u.data_cadastro.mes, &u.data_cadastro.ano) == 7) {
        if (total_usuarios < MAX_USUARIOS) {
            usuarios[total_usuarios++] = u;
        }
    }

    fclose(arquivo);
}

void cadastrarLivro() { //funcao para cadastrar um livro e salvar no arquivo livros.txt
    FILE *arquivo;
    struct livro livro = {0};

    printf("\n----- CADASTRO DE LIVRO ---------------------------------------------\n");

    printf("\nCodigo: ");
    scanf("%d", &livro.codigo);
    limparBuffer();

    printf("\nTitulo: ");
    fgets(livro.titulo, sizeof(livro.titulo), stdin);
    livro.titulo[strcspn(livro.titulo, "\n")] = '\0';

    printf("\nAutor: ");
    fgets(livro.autor, sizeof(livro.autor), stdin);
    livro.autor[strcspn(livro.autor, "\n")] = '\0';

    printf("\nEditora: ");
    fgets(livro.editora, sizeof(livro.editora), stdin);
    livro.editora[strcspn(livro.editora, "\n")] = '\0';

    printf("\nAno de publicacao: ");
    scanf("%d", &livro.ano);
    limparBuffer();

    printf("\nExemplares disponiveis: ");
    scanf("%d", &livro.exemplares);
    limparBuffer();

    if (livro.exemplares > 0) {
        livro.status = 1; //disponivel
    }
    else {
        livro.status = 0; //indisponivel
    }

    arquivo = fopen("livros.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo 'livros.txt'.\n");
        return;
    }

    fprintf(arquivo, "%d;%s;%s;%s;%d;%d;%d\n", livro.codigo, livro.titulo, livro.autor, livro.editora, livro.ano, livro.exemplares, livro.status);

    fclose(arquivo);
    printf("Livro salvo com sucesso em livros.txt!\n");
    carregarLivros();
}

void carregarLivros() { //funcao para carregar os dados de livros salvos caso exista um arquivo livros.txt
    FILE *arquivo = fopen("livros.txt", "r");
    if (!arquivo) {
        total_livros = 0;
        return;
    }

    total_livros = 0;
    struct livro l;

    while (fscanf(arquivo, "%d;%100[^;];%80[^;];%60[^;];%d;%d;%d\n", &l.codigo, l.titulo, l.autor, l.editora, &l.ano, &l.exemplares, &l.status) == 7) {
        if (total_livros < MAX_LIVROS) {
            livros[total_livros++] = l;
        }
    }

    fclose(arquivo);
}

void emprestarLivro() { //funcao para realizar um emprestimo de um livro e salvar no arquivo emprestimos.txt
    //verificacoes para saber se o usuario pode emprestar um livro
    if (total_livros <= 0) {
        printf("Primeiro tenha algum livro cadastrado para realizar um emprestimo.\n");
        return;
    }
    if (total_usuarios <= 0) {
        printf("Primeiro tenha algum usuario cadastrado para realizar um emprestimo.\n");
        return;
    }

    FILE *arquivo;
    struct emprestimo emprestimo = {0};
    
    printf("\n----- EMPRESTIMO DE LIVRO ---------------------------------------------\n");

    emprestimo.codigo = total_emprestimos + 1; //registra o codigo do emprestimo

    //variaveis para verificar a matricula
    int indice_usuario = -1;
    int matricula;
    char opcao; //variavel opcao para controle de fluxo

    do {
        printf("\nMatricula do usuario: ");
        scanf("%d", &matricula);
        limparBuffer();

        indice_usuario = buscarUsuarioMatricula(matricula); //verifica se a matricula e valida

        if (indice_usuario == -1) {
            printf("Matricula nao encontrada!\n");
            printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }
        }

    } while (indice_usuario == -1);

    emprestimo.matricula = matricula; //registra a matricula caso exista um usuario com ela

    //variaveis para verificar o codigo do livro
    int indice_livro = -1;
    int codigo_l;

    while (1) {
        printf("\nCodigo do livro: ");
        scanf("%d", &codigo_l);
        limparBuffer();

        indice_livro = buscarLivroCodigo(codigo_l); //verifica se o codigo e valido

        //se o codigo nao existe
        if (indice_livro == -1) {
            printf("Codigo do livro nao encontrado!\n");
            printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }

            continue; //tenta novamente
        }

        //se o livro existe, mas nao tem exemplares
        if (livros[indice_livro].exemplares <= 0) {
            printf("O livro nao possui exemplares disponiveis!\n");
            printf("Deseja tentar outro livro? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }

            continue; //volta a pedir o codigo do livro
        }

        break; //sai do loop se o livro existe e tem exemplares
    }

    emprestimo.codigo_livro = codigo_l; //registra o codigo do livro
    livros[indice_livro].exemplares--; //decrementa a quantidade de exemplares disponiveis dele

    if (livros[indice_livro].exemplares <= 0) {
        livros[indice_livro].status = 0; //atualiza o status do livro caso nao tenha exemplares disponiveis
    }

    int ok = 0; //variavel de controle para garantir que o usuario digite a data no formato correto

    while (!ok) { //enquanto a data nao estiver no formato correto, ela nao sera aceita
        printf("\nData do emprestimo (dd/mm/aaaa): ");

        if (scanf("%d/%d/%d", &emprestimo.data_emprestimo.dia, &emprestimo.data_emprestimo.mes, &emprestimo.data_emprestimo.ano) == 3 && dataValida(emprestimo.data_emprestimo)) {
            ok = 1;
            limparBuffer();
        }
        else {
            printf("Formato invalido! Use dd/mm/aaaa\n");
            limparBuffer();
        }
    }

    emprestimo.data_devolucao = calcularDataPrevista(emprestimo.data_emprestimo); //registra a data calculada para devolver o livro

    emprestimo.status = 1;

    arquivo = fopen("emprestimos.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo 'emprestimos.txt'.\n");
        return;
    }

    fprintf(arquivo, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n", emprestimo.codigo, emprestimo.matricula, emprestimo.codigo_livro,
        emprestimo.data_emprestimo.dia, emprestimo.data_emprestimo.mes, emprestimo.data_emprestimo.ano,
        emprestimo.data_devolucao.dia, emprestimo.data_devolucao.mes, emprestimo.data_devolucao.ano, emprestimo.status);

    fclose(arquivo);
    printf("Emprestimo salvo com sucesso em emprestimos.txt!\n");
    salvarLivros();
    carregarEmprestimos();
}

void carregarEmprestimos() { //funcao para carregar os dados de emprestimos salvos caso exista um arquivo emprestimos.txt
    FILE *arquivo = fopen("emprestimos.txt", "r");
    if (!arquivo) {
        total_emprestimos = 0;
        return;
    }

    total_emprestimos = 0;
    struct emprestimo e;

    while (fscanf(arquivo, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n", &e.codigo, &e.matricula, &e.codigo_livro,
                  &e.data_emprestimo.dia, &e.data_emprestimo.mes, &e.data_emprestimo.ano,
                  &e.data_devolucao.dia, &e.data_devolucao.mes, &e.data_devolucao.ano, &e.status) == 10) {
        if (total_emprestimos < MAX_EMPRESTIMOS) {
            emprestimos[total_emprestimos++] = e;
        }
    }

    fclose(arquivo);
}

void salvarLivros() { //funcao para regravar os livros apos ter modificado algo neles
    FILE *arquivo = fopen("livros.txt", "w");
    if (!arquivo) {
        printf("Erro ao salvar livros!\n");
        return;
    }

    for (int i = 0; i < total_livros; i++) {
        fprintf(arquivo, "%d;%s;%s;%s;%d;%d;%d\n", livros[i].codigo, livros[i].titulo, livros[i].autor,
                livros[i].editora, livros[i].ano, livros[i].exemplares, livros[i].status);
    }

    fclose(arquivo);
}

void devolverLivro() { //funcao para realizar a devolucao de um livro
    //verificacoes para saber se o usuario pode devolver um livro
    int ativo = 0;
    for (int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].status == 1) { //se existe um emprestimo ativo
            ativo = 1;
            break;
        }
    }

    if (!ativo) {
        printf("Primeiro tenha algum emprestimo ativo para realizar uma devolucao.\n");
        return;
    }

    printf("\n----- DEVOLUCAO DE LIVRO ---------------------------------------------\n");

    //variaveis para verificar o codigo do emprestimo
    int indice_emprestimo = -1;
    int codigo_e;
    char opcao;

    while (1) {
        printf("\nCodigo do emprestimo: ");
        scanf("%d", &codigo_e);
        limparBuffer();

        indice_emprestimo = buscarEmprestimoCodigo(codigo_e); //verifica se o codigo e valido

        //se o codigo nao existe
        if (indice_emprestimo == -1) {
            printf("Codigo do emprestimo nao encontrado!\n");
            printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }

            continue; //tenta novamente
        }

        //se o emprestimo existe, mas nao esta ativo
        if (emprestimos[indice_emprestimo].status <= 0) {
            printf("O livro relacionado a este emprestimo ja foi devolvido!\n");
            printf("Deseja tentar devolver outro livro? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }

            continue; //volta a pedir o codigo do emprestimo
        }

        break; //sai do loop se o emprestimo existe e esta ativo
    }

    emprestimos[indice_emprestimo].status = 0; //define o status como devolvido

    int indice_livro = buscarLivroCodigo(emprestimos[indice_emprestimo].codigo_livro); //captura o indice do livro devolvido
    livros[indice_livro].exemplares++; //incrementa o numero de exemplares do livro devolvido
    if (livros[indice_livro].exemplares > 0) {
        livros[indice_livro].status = 1; //define o status do livro devolvido como disponivel
    }

    printf("Livro devolvido com sucesso!");
    salvarLivros();
    salvarEmprestimos();

}

void salvarEmprestimos() { //funcao para regravar os emprestimos apos ter modificado algo neles
    FILE *arquivo = fopen("emprestimos.txt", "w");
    if (!arquivo) {
        printf("Erro ao salvar emprestimos!\n");
        return;
    }

    for (int i = 0; i < total_emprestimos; i++) {
        fprintf(arquivo, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n", emprestimos[i].codigo, emprestimos[i].matricula, emprestimos[i].codigo_livro,
        emprestimos[i].data_emprestimo.dia, emprestimos[i].data_emprestimo.mes, emprestimos[i].data_emprestimo.ano,
        emprestimos[i].data_devolucao.dia, emprestimos[i].data_devolucao.mes, emprestimos[i].data_devolucao.ano, emprestimos[i].status);
    }

    fclose(arquivo);

}

void pesquisarLivros() { //funcao para pesquisar livros por codigo, titulo ou autor e exibir as informacoes encontradas
    if (total_livros <= 0) { //verificacao para saber se o usuario pode pesquisar livros
        printf("Primeiro tenha algum livro cadastrado para pesquisar livros.\n");
        return;
    }

    printf("\n----- PESQUISAR LIVROS ---------------------------------------------\n");

    int opcao = 0; //opcao de busca
    int indice_livro = -1; //indice do livro buscado
    char opc; //opcao para controle de fluxo
    while (1) {
        printf("\nDigite como deseja buscar livros (1 = por codigo / 2 = por titulo / 3 = por autor): ");
        scanf("%d", &opcao);
        limparBuffer();

        if (opcao != 1 && opcao != 2 && opcao != 3) {
            printf("Opcao invalida!\n");
            continue; //reinicia o loop
        }

        if (opcao == 1) { //busca por codigo
            int codigo;
            printf("\nDigite o codigo: ");
            scanf("%d", &codigo);
            limparBuffer();

            indice_livro = buscarLivroCodigo(codigo);
            if (indice_livro == -1) {
                printf("Codigo nao encontrado!\n");
                printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
                scanf("%c", &opc);
                limparBuffer();

                if (opc == 's' || opc == 'S') {
                    continue;                    
                }
                else {
                    return;
                }
            }
        }

        if (opcao == 2) { //busca por titulo
            char titulo[101];
            printf("\nDigite o titulo: ");
            fgets(titulo, sizeof(titulo), stdin);
            titulo[strcspn(titulo, "\n")] = '\0';

            indice_livro = buscarLivroTitulo(titulo);
            if (indice_livro == -1) {
                printf("Titulo nao encontrado!\n");
                printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
                scanf("%c", &opc);
                limparBuffer();

                if (opc == 's' || opc == 'S') {
                    continue;                    
                }
                else {
                    return;
                }
            }
        }

        if (opcao == 3) { //busca por autor
            char autor[81];
            int encontrados[100]; //array que recebera os indices dos livros do mesmo autor

            printf("\nDigite o autor: ");
            fgets(autor, sizeof(autor), stdin);
            autor[strcspn(autor, "\n")] = '\0';

            int qtd = buscarLivrosAutor(autor, encontrados);

            if (qtd <= 0) {
                printf("Nenhum livro encontrado!\n");
                printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
                scanf("%c", &opc);
                limparBuffer();

                if (opc == 's' || opc == 'S') {
                    continue;                    
                }
                else {
                    return;
                }
            }
            else {
                printf("\nLivros encontrados:");
                for (int i = 0; i < qtd; i++) {
                    int idx = encontrados[i];
                    printf("\nCodigo: %d\n", livros[idx].codigo);
                    printf("Titulo: %s\n", livros[idx].titulo);
                    printf("Autor: %s\n", livros[idx].autor);
                    printf("Editora: %s\n", livros[idx].editora);
                    printf("Ano de publicacao: %04d\n", livros[idx].ano);
                    printf("Exemplares disponiveis: %d\n", livros[idx].exemplares);
                    if (livros[idx].status > 0) {
                        printf("Status: disponivel para emprestimo\n");
                    }
                    else {
                        printf("Status: indisponivel para emprestimo\n");
                    }
                }
            }
        }
        else {
            printf("\nLivro encontrado:");
            printf("\nCodigo: %d\n", livros[indice_livro].codigo);
            printf("Titulo: %s\n", livros[indice_livro].titulo);
            printf("Autor: %s\n", livros[indice_livro].autor);
            printf("Editora: %s\n", livros[indice_livro].editora);
            printf("Ano de publicacao: %04d\n", livros[indice_livro].ano);
            printf("Exemplares disponiveis: %d\n", livros[indice_livro].exemplares);
            if (livros[indice_livro].status > 0) {
                printf("Status: disponivel para emprestimo\n");
            }
            else {
                printf("Status: indisponivel para emprestimo\n");
            }
        }

        
        printf("\nDeseja buscar outro livro? (s para buscar / n para sair): ");
        scanf("%c", &opc);
        limparBuffer();

        if (opc == 's' || opc == 'S') {
            continue;
        }

        break;
    }
}

int buscarLivroCodigo(int codigo) { //funcao que procura um livro por codigo e retorna o indice dele no vetor livros
    for (int i = 0; i < total_livros; i++) {
        if (livros[i].codigo == codigo) {
            return i; //retorna o indice
        }
    }
    return -1; //nao encontrado
}

int buscarLivroTitulo(const char *titulo) { //funcao que procura um livro por titulo e retorna o indice dele no vetor livros
    for (int i = 0; i < total_livros; i++) {
        if (strcmp(livros[i].titulo, titulo) == 0) { 
            return i;
        }
    }
    return -1; //nao encontrado
}

int buscarLivrosAutor(const char *autor, int indices[]) { //funcao que procura livros por autor e retorna a quantidade de livros dele
    int qtd = 0;

    for (int i = 0; i < total_livros; i++) {
        if (strcmp(livros[i].autor, autor) == 0) {
            indices[qtd] = i; //salva o indice encontrado
            qtd++;
        }
    }

    return qtd; //quantidade de livros encontrados
}

void pesquisarUsuarios() { //funcao para pesquisar usuarios por nome ou matricula e exibir as informacoes encontradas
    if (total_usuarios <= 0) { //verificacao para saber se o usuario pode pesquisar usuarios
        printf("Primeiro tenha algum usuario cadastrado para pesquisar usuarios.\n");
        return;
    }

    printf("\n----- PESQUISAR USUARIOS ---------------------------------------------\n");

    int opcao = 0; //opcao de busca
    int indice_usuario = -1; //indice do usuario buscado
    char opc; //opcao para controle de fluxo
    while (1) {
        printf("\nDigite como deseja buscar usuario (1 = por matricula / 2 = por nome): ");
        scanf("%d", &opcao);
        limparBuffer();

        if (opcao != 1 && opcao != 2) {
            printf("Opcao invalida!\n");
            continue; //reinicia o loop
        }

        if (opcao == 1) {
            int matricula;
            printf("\nDigite a matricula: ");
            scanf("%d", &matricula);
            limparBuffer();

            indice_usuario = buscarUsuarioMatricula(matricula);
            if (indice_usuario == -1) {
                printf("Matricula nao encontrada!\n");
                printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
                scanf("%c", &opc);
                limparBuffer();

                if (opc == 's' || opc == 'S') {
                    continue;                    
                }
                else {
                    return;
                }
            }
        }

        if (opcao == 2) {
            char nome[101];
            printf("\nDigite o nome: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';

            indice_usuario = buscarUsuarioNome(nome);
            if (indice_usuario == -1) {
                printf("Nome nao encontrado!\n");
                printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
                scanf("%c", &opc);
                limparBuffer();

                if (opc == 's' || opc == 'S') {
                    continue;                    
                }
                else {
                    return;
                }
            }
        }

        printf("\nUsuario encontrado:");
        printf("\nNome completo: %s\n", usuarios[indice_usuario].nome);
        printf("Matricula: %d\n", usuarios[indice_usuario].matricula);
        printf("Curso: %s\n", usuarios[indice_usuario].curso);
        printf("Telefone: %s\n", usuarios[indice_usuario].telefone);
        printf("Data de cadastro: %02d/%02d/%04d\n", usuarios[indice_usuario].data_cadastro.dia,
               usuarios[indice_usuario].data_cadastro.mes, usuarios[indice_usuario].data_cadastro.ano);
        
        printf("\nDeseja buscar outro usuario? (s para buscar / n para sair): ");
        scanf("%c", &opc);
        limparBuffer();

        if (opc == 's' || opc == 'S') {
            continue;
        }

        break;
    }
}

int buscarUsuarioMatricula(int matricula) { //funcao que procura um usuario por matricula e retorna o indice dele no vetor usuarios
    for (int i = 0; i < total_usuarios; i++) {
        if (usuarios[i].matricula == matricula) {
            return i; //retorna o indice
        }
    }
    return -1; //nao encontrado
}

int buscarUsuarioNome(const char *nome) { //funcao que procura um usuario por nome e retorna o indice dele no vetor usuarios
    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].nome, nome) == 0) { 
            return i;
        }
    }
    return -1; //nao encontrado
}

int buscarEmprestimoCodigo(int codigo) { //funcao que procura um emprestimo por codigo e retorna o indice dele no vetor emprestimos
    for (int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].codigo == codigo) {
            return i; //retorna o indice
        }
    }
    return -1; //nao encontrado
}

void listarEmprestimosAtivos() { //funcao para listar emprestimos que estao ativos
    if (total_emprestimos <= 0) { //verificacoes para saber se o usuario pode listar emprestimos
        printf("Primeiro tenha algum emprestimo cadastrado para poder listar.\n");
        return;
    }
    int ativo = 0;
    for (int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].status == 1) {
            ativo = 1;
            break;
        }
    }
    if (!ativo) {
        printf("Nenhum emprestimo ativo no momento!\n");
        return;
    }

    printf("\n----- EMPRESTIMOS ATIVOS ---------------------------------------------\n");

    for (int i = 0; i < total_emprestimos; i++) {
        if (emprestimos[i].status == 1) { //se estiver ativo exibe as informacoes
            printf("\nCodigo do emprestimo: %d\n", emprestimos[i].codigo);
            printf("Matricula vinculada: %d\n", emprestimos[i].matricula);
            printf("Codigo do livro: %d\n", emprestimos[i].codigo_livro);
            printf("Data do emprestimo: %02d/%02d/%04d\n", emprestimos[i].data_emprestimo.dia, emprestimos[i].data_emprestimo.mes, emprestimos[i].data_emprestimo.ano);
            printf("Data prevista para devolucao: %02d/%02d/%04d\n", emprestimos[i].data_devolucao.dia, emprestimos[i].data_devolucao.mes, emprestimos[i].data_devolucao.ano);
            printf("\n----------------------------------\n");
        }
    }

    char opc;
    printf("\nDigite s para voltar: ");
    scanf("%c", &opc);
    limparBuffer();
}

void renovarEmprestimo() { //funcao para renovar o emprestimo de um livro
    if (total_emprestimos <= 0) { //verificacoes para saber se o usuario pode renovar emprestimos
        printf("Primeiro tenha algum emprestimo cadastrado para poder renovar.\n");
        return;
    }

    printf("\n----- RENOVACAO DE EMPRESTIMO ---------------------------------------------\n");

    //variaveis para verificar o codigo do emprestimo
    int indice_emprestimo = -1;
    int codigo_e;
    char opcao;

    while (1) {
        printf("\nCodigo do emprestimo: ");
        scanf("%d", &codigo_e);
        limparBuffer();

        indice_emprestimo = buscarEmprestimoCodigo(codigo_e); //verifica se o codigo e valido

        //se o codigo nao existe
        if (indice_emprestimo == -1) {
            printf("Codigo do emprestimo nao encontrado!\n");
            printf("Deseja tentar novamente? (s para tentar / n para voltar): ");
            scanf("%c", &opcao);
            limparBuffer();

            if (opcao == 'n' || opcao == 'N') {
                return; //volta ao menu
            }

            continue; //tenta novamente
        }

        if (emprestimos[indice_emprestimo].status <= 0) { //codigo existe mas se nao estiver ativo
            int indice_livro = buscarLivroCodigo(emprestimos[indice_emprestimo].codigo_livro);
            if (livros[indice_livro].exemplares <= 0) {
                printf("\nO livro cadastrado neste emprestimo nao possui exemplares!\n");
                printf("Deseja tentar renovar outro emprestimo? (s para tentar / n para voltar): ");
                scanf("%c", &opcao);
                limparBuffer();

                if (opcao == 'n' || opcao == 'N') {
                    return; //volta ao menu
                }

                continue; //tenta novamente
            }

            int ok = 0; //variavel de controle para garantir que o usuario digite a data no formato correto

            while (!ok) { //enquanto a data nao estiver no formato correto, ela nao sera aceita
                printf("\nO emprestimo esta inativo, digite a data para renovacao (dd/mm/aaaa): ");

                if (scanf("%d/%d/%d", &emprestimos[indice_emprestimo].data_emprestimo.dia, &emprestimos[indice_emprestimo].data_emprestimo.mes,
                    &emprestimos[indice_emprestimo].data_emprestimo.ano) == 3 && dataValida(emprestimos[indice_emprestimo].data_emprestimo)) {
                    ok = 1;
                    limparBuffer();
                }
                else {
                    printf("Formato invalido! Use dd/mm/aaaa\n");
                    limparBuffer();
                }
            }

            //renova a data de devolucao, status e exemplares disponiveis do livro emprestado
            emprestimos[indice_emprestimo].data_devolucao = calcularDataPrevista(emprestimos[indice_emprestimo].data_emprestimo);
            emprestimos[indice_emprestimo].status = 1;
            livros[indice_livro].exemplares--;
            if (livros[indice_livro].exemplares <= 0) { //se o livro nao possuir mais exemplares, status fica indisponivel
                livros[indice_livro].status = 0;
            }
            salvarLivros();
        }
        else { //se estiver ativo adiciona mais 7 dias de emprestimo
            data data = emprestimos[indice_emprestimo].data_devolucao;
            emprestimos[indice_emprestimo].data_devolucao = calcularDataPrevista(data);
        }

        break; //sai do loop
    }

    printf("Emprestimo renovado com sucesso!");
    salvarEmprestimos();
}