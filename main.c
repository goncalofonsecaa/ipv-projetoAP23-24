#include <stdio.h>
#include <math.h>
#include <locale.h>   // para usar caracteres acentuados
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int MAX_SERVICOS = 2000;
const int BUFFER_SIZE = 1056;

// Estrutura para representar a informação de um quarto
typedef struct {
    int torre;
    int andar;
    int numero;
    int quarto_codigo;
    char vista;
    char qualidade;
    char max_pessoas;
    int disponibilidade; // 1 se disponível, 0 se não disponível
} Quarto;

// Estrutura para representar a ficha de cliente provisória
typedef struct {
    int codigo;
    char nome[50];
    char morada[100];
    char codigo_postal[15];
    char localidade[50];
    char nif[15];
    char data_criacao[15];
    char tipo_cliente; // 'N' para Normal, 'V' para VIP, 'E' para Excelência
    // Outros campos necessários
} ClienteProvisorio;

// Estrutura para representar a reserva
typedef struct {
    int num_reserva;
    int Nif_Cliente; // Código do cliente (provisório ou definitivo)
    char tipo_ficha_cliente; // 'P' para Provisória, 'D' para Definitiva
    char quarto_codigo[9]; // {torre}0{andar}0{numero}
    char data_checkin[15];
    char data_checkout[15];
    int num_pessoas;
    int quartos_adicionais[5]; // Máximo de 5 quartos associados a este cliente
    float valor_pago;
    float valor_total;
    char forma_pagamento[20];
    char situacao_reserva; // 'P' para Provisória, 'C' para Confirmada, 'O' para Em Operação, 'F' para Finalizada
    char observacoes[200];
    // Outros campos necessários
} Reserva;

// Estrutura para representar os serviços complementares contratados
typedef struct {
    Quarto quarto_codigo;
    int funcionario_registo;
    char data_hora_registo[25];
    char tipo_servico[20];
    char codigo_servico[20];
    char observacoes[200];
    // Outros campos necessários
} Servico;

// Estrutura para representar a tabela de preços base
typedef struct {
    char vista;
    char qualidade;
    char max_pessoas;
    float valor;
    float valor_cancelamento;
    char inicio_promocao[10];
    char fim_promocao[10];
    float valor_promocao;
} TabelaPrecosBase;

// Estrutura para representar a tabela de adicional aos valores base conforme a época
typedef struct {
    //tipo de época
    char TipoEpoca[10];
    char epoca_inicio[10];
    char epoca_fim[10];
    float percentagem_adicional;
} TabelaAdicionalEpoca;

// Estrutura para representar a tabela de valores dos serviços complementares
typedef struct {
    char tipo_servico[50];
    float valor;
} TabelaServicos;

// Estrutura para representar o hotel
typedef struct {
    Quarto quartos[2][10][200]; // 2 torres, 10 andares, 200 quartos cada
    ClienteProvisorio clientes_provisorios[1000]; // Máximo de 1000 clientes provisórios
    Reserva reservas[1000]; // Máximo de 1000 reservas
    Servico servicos_contratados[2000]; // Máximo de 2000 serviços contratados
    TabelaPrecosBase precos_base[16]; // 16 combinações possíveis de vista, qualidade e número de pessoas
    TabelaAdicionalEpoca adicional_epoca[4]; // 4 épocas possíveis (Baixa, Intermediária, Média, Alta)
    TabelaServicos servicos_complementares[10]; // 10 tipos de serviços complementares
    // Outros campos necessários
} Hotel;
 
// Estrutura para representar os dados de pagamento
typedef struct {
    int num_reserva;
    float valor_pago;
} Pagamento;


void registrarQuarto(Quarto *lista_quartos, int *n_quartos) {
  Quarto quarto;

  printf("Digite a torre (1 ou 2): ");
  scanf("%d", &quarto.torre);

  if (quarto.torre < 1 || quarto.torre > 2) {
    printf("Entrada inválida para torre.\n");
    return;
  }

  printf("Digite o andar (1 a 10): ");
  scanf("%d", &quarto.andar);

  if (quarto.andar < 1 || quarto.andar > 10) {
    printf("Entrada inválida para andar.\n");
    return;
  }

  printf("Digite o número do quarto (1 a 200): ");
  scanf("%d", &quarto.numero);

  if (quarto.numero < 1 || quarto.numero > 200) {
    printf("Entrada inválida para número do quarto.\n");
    return;
  }


  // Verifica se o quarto já existe na lista
  for (int indiceQuarto = 0; indiceQuarto < *n_quartos; indiceQuarto++) {
    if (lista_quartos[indiceQuarto].torre == quarto.torre &&
      lista_quartos[indiceQuarto].andar == quarto.andar &&
      lista_quartos[indiceQuarto].numero == quarto.numero) {
      printf("Já existe um quarto registado com esses dados. Registo cancelado.\n");
      return;
    }
  }

  printf ("Qual a vista do quarto (M ou P): ");
  scanf (" %c", &quarto.vista);

  if (quarto.vista != 'M' && quarto.vista != 'P') {
    printf("Entrada inválida para vista.\n");
    return;
  }
  
  printf ("Qual a qualidade do quarto (S ou B): ");
  scanf (" %c", &quarto.qualidade);
  
  //qualidade so pode ser S OU B 
  if (quarto.qualidade != 'S' && quarto.qualidade != 'B') {
    printf("Entrada inválida para qualidade.\n");
    return;
  }
  
  printf ("Qual o número máximo de pessoas admitidas no quarto (S, D , T ou F): ");
  scanf (" %c", &quarto.max_pessoas);

  //max de pessoas pode ser S, D , T ou F 
  
  if (quarto.max_pessoas != 'S' && quarto.max_pessoas != 'D' && quarto.max_pessoas != 'T' && quarto.max_pessoas != 'F') {
    printf("Entrada inválida para número máximo de pessoas admitidas no quarto.\n");
    return;
  }

  quarto.disponibilidade = 1;

  lista_quartos[*n_quartos] = quarto;
  (*n_quartos)++;

  printf("Quarto registado com sucesso!\n");
}


void escreverFichQuartos(Quarto *lista_quartos, int n_quartos) {
    FILE *fich_quartos = fopen("quartos.txt", "w+");

    if (fich_quartos == NULL) {
        printf("Não foi possível abrir o ficheiro de quartos!\n");
        return;
    }

    fprintf(fich_quartos, "Torre\tAndar\tNumero\tVista\tQualidade\tMax_Pessoas\tDisponibilidade\n");

    for (int indiceQuarto = 0; indiceQuarto < n_quartos; indiceQuarto++) {
        fprintf(fich_quartos, "%d\t%d\t%d\t%c\t%c\t%c\t%d\n", lista_quartos[indiceQuarto].torre, lista_quartos[indiceQuarto].andar, lista_quartos[indiceQuarto].numero, lista_quartos[indiceQuarto].vista, lista_quartos[indiceQuarto].qualidade, lista_quartos[indiceQuarto].max_pessoas, lista_quartos[indiceQuarto].disponibilidade);
    }

    fclose(fich_quartos);
}

void lerFichQuartos(Quarto *quarto, int *n_quartos) {
    FILE *fich_quartos = fopen("quartos.txt", "r");

    if (fich_quartos == NULL) {
        printf("Não foi possível abrir o ficheiro de quartos!\n");
        return;
    }

    char buffer[BUFFER_SIZE];
    int linha_cont = 0; //contador de linhas

    // serve para ler a primeira linha de texto (cabeçalho) e ignorar
    fgets(buffer, BUFFER_SIZE, fich_quartos);

    while (fgets(buffer, BUFFER_SIZE, fich_quartos) != NULL && *n_quartos < MAX_SERVICOS - 1) {
        char linha[BUFFER_SIZE];
        strcpy(linha, buffer);

        char *torre = strtok(linha, "\t");
        char *andar = strtok(NULL, "\t");
        char *numero = strtok(NULL, "\t");
        char *vista = strtok(NULL, "\t");
        char *qualidade = strtok(NULL, "\t");
        char *max_pessoas = strtok(NULL, "\t");
        char *disponibilidade = strtok(NULL, "\t");

        if (torre == NULL || andar == NULL || numero == NULL || vista == NULL || qualidade == NULL || max_pessoas == NULL || disponibilidade == NULL) {
            fprintf(stderr, "Erro na leitura dos campos do ficheiro de quartos\n");
            return;
        }

        quarto[*n_quartos].torre = atoi(torre);
        quarto[*n_quartos].andar = atoi(andar);
        quarto[*n_quartos].numero = atoi(numero);
        quarto[*n_quartos].vista = *vista;
        quarto[*n_quartos].qualidade = *qualidade;
        quarto[*n_quartos].max_pessoas = *max_pessoas;
        quarto[*n_quartos].disponibilidade = atoi(disponibilidade);

        (*n_quartos)++;
        linha_cont++;

        if (linha_cont >= MAX_SERVICOS) {
            fprintf(stderr, "Foi atingido o numero máximo de quartos!\n");
            return;
        }

        if (*n_quartos >= MAX_SERVICOS) {
            fprintf(stderr, "Foi atingido o numero maximo de quartos!\n");
            return;
        }
    }

    fclose(fich_quartos);

    printf("Ficheiro de quartos lido com sucesso!\n");
}

void alterarQuarto(Quarto *quartos, int n_quartos) {
  int torre, andar, numero;

  printf("Qual é a torre do quarto (1 ou 2): ");
  scanf("%d", &torre);

  if (torre < 1 || torre > 2) {
    printf("Entrada inválida para torre.\n");
    return;
  }

  printf("Qual é o andar do quarto (1 a 10): ");
  scanf("%d", &andar);

  if (andar < 1 || andar > 10) {
    printf("Entrada inválida para andar.\n");
    return;
  }

  printf("Qual é o número do quarto (1 a 200): ");
  scanf("%d", &numero);

  if (numero < 1 || numero > 200) {
    printf("Entrada inválida para número do quarto.\n");
    return;
  }

  int indice = -1;

  for (int indiceQuarto = 0; indiceQuarto < n_quartos; indiceQuarto++) {
    if (quartos[indiceQuarto].torre == torre &&
      quartos[indiceQuarto].andar == andar &&
      quartos[indiceQuarto].numero == numero) {
      indice = indiceQuarto;
      break;
    }
  }

  if (indice == -1) {
    printf("Não existe nenhum quarto com esses dados.\n");
    return;
  }

  int campo = -1;

  do {
    printf("Qual campo pretende alterar?\n");
    printf("1 - Torre\n");
    printf("2 - Andar\n");
    printf("3 - Número\n");
    printf("4 - Vista\n");
    printf("5 - Qualidade\n");
    printf("6 - Máximo de pessoas\n");
    printf("0 - Sair\n");
    printf("Opção: ");
    scanf("%d", &campo);
  } while (campo < 0 || campo > 6);

  if (campo == 0) {
    return;
  }

  switch (campo) {
    case 1: {
      printf("Qual é a nova torre do quarto (1 ou 2): ");
      scanf("%d", &quartos[indice].torre);

      if (quartos[indice].torre < 1 || quartos[indice].torre > 2) {
        printf("Entrada inválida para torre.\n");
        return;
      }
      break;
    }
    
    case 2:  {
      printf("Qual é o novo andar do quarto (1 a 10): ");
      scanf("%d", &quartos[indice].andar);

      if (quartos[indice].andar < 1 || quartos[indice].andar > 10) {
        printf("Entrada inválida para andar.\n");
        return;
      }
      break;
    }

    case 3: {
      printf("Qual é o novo número do quarto (1 a 200): ");
      scanf("%d", &quartos[indice].numero);

      if (quartos[indice].numero < 1 || quartos[indice].numero > 200) {
        printf("Entrada inválida para número do quarto.\n");
        return;
      }
      break;
    }

    case 4: {
      printf("Qual é a nova vista do quarto (M ou P): ");
      scanf(" %c", &quartos[indice].vista);

      if (quartos[indice].vista != 'M' && quartos[indice].vista != 'P') {
        printf("Entrada inválida para vista.\n");
        return;
      }
      break;
    }

    case 5: {
      printf("Qual é a nova qualidade do quarto (S ou B): ");
      scanf(" %c", &quartos[indice].qualidade);

      if (quartos[indice].qualidade != 'S' && quartos[indice].qualidade != 'B') {
        printf("Entrada inválida para qualidade.\n");
        return;
      }
      break;
    }

    case 6: {
      printf("Qual é o novo número máximo de pessoas admitidas no quarto: ");
      scanf(" %c", &quartos[indice].max_pessoas);

      if (quartos[indice].max_pessoas != 'S' && quartos[indice].max_pessoas != 'D' && quartos[indice].max_pessoas != 'T' && quartos[indice].max_pessoas != 'F') {
        printf("Entrada inválida para número máximo de pessoas admitidas no quarto.\n");
        return;
      }
      break;
    }
  }

  printf("Quarto alterado com sucesso!\n");
}

//listar quartos

void listarQuartos(Quarto *lista_quartos, int n_quartos) {
  for (int indiceQuarto = 0; indiceQuarto < n_quartos; indiceQuarto++) {
    printf("Torre: %d\n", lista_quartos[indiceQuarto].torre);
    printf("Andar: %d\n", lista_quartos[indiceQuarto].andar);
    printf("Número: %d\n", lista_quartos[indiceQuarto].numero);
    printf("Vista: %c\n", lista_quartos[indiceQuarto].vista);
    printf("Qualidade: %c\n", lista_quartos[indiceQuarto].qualidade);
    printf("Máximo de pessoas: %c\n", lista_quartos[indiceQuarto].max_pessoas);
    printf("Disponibilidade: %d\n", lista_quartos[indiceQuarto].disponibilidade);
    printf("\n");
  }
}




void registarServico(TabelaServicos *listaServicos, int *num_servicos) {
  TabelaServicos servico;

  printf("Qual o tipo de serviço: ");
  scanf("%s", servico.tipo_servico);

  printf("Qual o valor do serviço: ");
  scanf("%f", &servico.valor);

  listaServicos[*num_servicos] = servico;
  (*num_servicos)++;

  printf("Serviço registado com sucesso!\n");
}

//funçao para remover um serviço por id
void removerServico(TabelaServicos *listaServicos, int *num_servicos) {
  char tipo_servico[50];

  printf("Qual o tipo de serviço que pretende remover: ");
  scanf("%s", tipo_servico);

  int indice = -1;

  for (int indiceServico = 0; indiceServico < *num_servicos; indiceServico++) {
    if (strcmp(listaServicos[indiceServico].tipo_servico, tipo_servico) == 0) {
      indice = indiceServico;
      break;
    }
  }

  if (indice == -1) {
    printf("Não existe nenhum serviço com esse id.\n");
    return;
  }

  for (int indiceServico = indice; indiceServico < *num_servicos - 1; indiceServico++) {
    listaServicos[indiceServico] = listaServicos[indiceServico + 1];
  }

  (*num_servicos)--;

  printf("Serviço removido com sucesso!\n");
}

//lerFichServicos

void lerFichServicos(TabelaServicos *servico, int *num_servicos) {
  FILE *fich_servicos = fopen("servicos.txt", "r");

  if (fich_servicos == NULL) {
    printf("Não foi possível abrir o ficheiro de serviços!\n");
    return;
  }

  char buffer[BUFFER_SIZE];
  int linha_cont = 0; //contador de linhas

  // serve para ler a primeira linha de texto (cabeçalho) e ignorar

  fgets(buffer, BUFFER_SIZE, fich_servicos);

  while (fgets(buffer, BUFFER_SIZE, fich_servicos) != NULL && *num_servicos <= MAX_SERVICOS -1) {
    char linha[BUFFER_SIZE];
    strcpy(linha, buffer);

    char *tipo_servico = strtok(linha, "\t");
    char *valor = strtok(NULL, "\t");

    if (*tipo_servico == NULL || *valor == NULL) {
      fprintf(stderr, "Erro na leitura dos campos do ficheiro de serviços\n");
      return;
    }

    strcpy(servico[*num_servicos].tipo_servico, tipo_servico);
    servico[*num_servicos].valor = atof(valor);

    (*num_servicos)++;
    linha_cont++;

    if (linha_cont > MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero máximo de serviços!\n");
      return;
    }

    if (*num_servicos > MAX_SERVICOS) {
      fprintf(stderr, " Foi atingido o numero maximo de serviços!\n");
      return;
    }
  }

  fclose(fich_servicos);

  printf("Ficheiro de serviços lido com sucesso!\n");
}

//escreverFichServicos

void escreverFichServicos(TabelaServicos listaServicos[], int num_servicos) {
  FILE *fich_servicos = fopen("servicos.txt", "w+");

  if (fich_servicos == NULL) {
    printf("Não foi possível abrir o ficheiro de serviços!\n");
    return;
  }

  fprintf(fich_servicos, "Tipo\tValor\n");

  for (int indiceServico = 0; indiceServico < num_servicos; indiceServico++) {
    fprintf(fich_servicos, "%s\t%.2f\n", listaServicos[indiceServico].tipo_servico, listaServicos[indiceServico].valor);
  }

  fclose(fich_servicos);
}

void alterarServico (TabelaServicos *listaServicos, int *num_servicos) {
  char tipo_servico[50];

  printf("Qual o tipo de serviço que pretende alterar: ");
  scanf("%s", tipo_servico);

  int indice = -1;

  for (int indiceServico = 0; indiceServico < *num_servicos; indiceServico++) {
    if (strcmp(listaServicos[indiceServico].tipo_servico, tipo_servico) == 0) {
      indice = indiceServico;
      break;
    }
  }

  if (indice == -1) {
    printf("Não existe nenhum serviço com esse id.\n");
    return;
  }

  int campo = -1;

  do {
    printf("Qual campo pretende alterar?\n");
    printf("1 - Tipo\n");
    printf("2 - Valor\n");
    printf("0 - Sair\n");
    printf("Opção: ");
    scanf("%d", &campo);
  } while (campo < 0 || campo > 2);

  if (campo == 0) {
    return;
  }

  switch (campo) {
    case 1: {
      printf("Qual é o novo tipo de serviço: ");
      scanf("%s", listaServicos[indice].tipo_servico);
      break;
    }
    
    case 2:  {
      printf("Qual é o novo valor do serviço: ");
      scanf("%f", &listaServicos[indice].valor);
      break;
    }
  }

  printf("Serviço alterado com sucesso!\n");
}

//funçao para listar serviços

void listarServicos(TabelaServicos *listaServicos, int num_servicos) {
  for (int indiceServico = 0; indiceServico < num_servicos; indiceServico++) {
    printf("Tipo: %s\n", listaServicos[indiceServico].tipo_servico);
    printf("Valor: %.2f\n", listaServicos[indiceServico].valor);
    printf("\n");
  }
}


//Comeca Reserva AQUI


//criar ficha cliente provisorio 

void criarFichaClienteProvisorio(ClienteProvisorio *listaClientesProvisorios, int *n_clientes_provisorios) {
  ClienteProvisorio cliente_provisorio;

  printf("Qual o nome do cliente: ");
  scanf("  %s", cliente_provisorio.nome);

  printf("Qual a morada do cliente: ");
  scanf("  %s", cliente_provisorio.morada);

  printf("Qual o código postal do cliente: ");
  scanf("  %s", cliente_provisorio.codigo_postal);

  printf("Qual a localidade do cliente: ");
  scanf(" %s", cliente_provisorio.localidade);

  printf("Qual o NIF do cliente: ");
  scanf("  %s", cliente_provisorio.nif);

  printf("Qual a data de criação da ficha do cliente: ");
  scanf("  %s", cliente_provisorio.data_criacao);

  printf("Qual o tipo de cliente (N, V ou E): ");
  scanf("  %c", &cliente_provisorio.tipo_cliente);

  listaClientesProvisorios[*n_clientes_provisorios] = cliente_provisorio;
  (*n_clientes_provisorios)++;

  printf("Ficha de cliente provisório criada com sucesso!\n");
}

//escrever no ficheiro ficha cliente provisorio

void escreverFichClienteProv (ClienteProvisorio *listaClientesProvisorios, int n_clientes_provisorios) {
  FILE *fich_clientes_provisorios = fopen("clientes_provisorios.txt", "w+");

  if (fich_clientes_provisorios == NULL) {
    printf("Não foi possível abrir o ficheiro de clientes provisórios!\n");
    return;
  }

  fprintf(fich_clientes_provisorios, "Nome\tMorada\tCodigo_Postal\tLocalidade\tNIF\tData_Criacao\tTipo_Cliente\n");

  for (int indiceClienteProvisorio = 0; indiceClienteProvisorio < n_clientes_provisorios; indiceClienteProvisorio++) {
    fprintf(fich_clientes_provisorios, "%s\t%s\t%s\t%s\t%s\t%s\t%c\n", listaClientesProvisorios[indiceClienteProvisorio].nome, listaClientesProvisorios[indiceClienteProvisorio].morada, listaClientesProvisorios[indiceClienteProvisorio].codigo_postal, listaClientesProvisorios[indiceClienteProvisorio].localidade, listaClientesProvisorios[indiceClienteProvisorio].nif, listaClientesProvisorios[indiceClienteProvisorio].data_criacao, listaClientesProvisorios[indiceClienteProvisorio].tipo_cliente);
  }

  fclose(fich_clientes_provisorios);
}

//ler ficheiro ficha cliente provisorio sem limite de registos

void lerFichClientesProv (ClienteProvisorio *cliente_provisorio, int *n_clientes_provisorios) {
  FILE *fich_clientes_provisorios = fopen("clientes_provisorios.txt", "r");

  if (fich_clientes_provisorios == NULL) {
    printf("Não foi possível abrir o ficheiro de clientes provisórios!\n");
    return;
  }

  char buffer[BUFFER_SIZE];
  int linha_cont = 0; //contador de linhas

  // serve para ler a primeira linha de texto (cabeçalho) e ignorar
  fgets(buffer, BUFFER_SIZE, fich_clientes_provisorios);

  while (fgets(buffer, BUFFER_SIZE, fich_clientes_provisorios) != NULL && *n_clientes_provisorios < MAX_SERVICOS - 1) {
    char linha[BUFFER_SIZE];
    strcpy(linha, buffer);

    char *nome = strtok(linha, "\t");
    char *morada = strtok(NULL, "\t");
    char *codigo_postal = strtok(NULL, "\t");
    char *localidade = strtok(NULL, "\t");
    char *nif = strtok(NULL, "\t");
    char *data_criacao = strtok(NULL, "\t");
    char *tipo_cliente = strtok(NULL, "\t");

    if (nome == NULL || morada == NULL || codigo_postal == NULL || localidade == NULL || nif == NULL || data_criacao == NULL || tipo_cliente == NULL) {
      fprintf(stderr, "Erro na leitura dos campos do ficheiro de clientes\n");
      return;
    }

    strcpy(cliente_provisorio[*n_clientes_provisorios].nome, nome);
    strcpy(cliente_provisorio[*n_clientes_provisorios].morada, morada);
    strcpy(cliente_provisorio[*n_clientes_provisorios].codigo_postal, codigo_postal);
    strcpy(cliente_provisorio[*n_clientes_provisorios].localidade, localidade);
    strcpy(cliente_provisorio[*n_clientes_provisorios].nif, nif);
    strcpy(cliente_provisorio[*n_clientes_provisorios].data_criacao, data_criacao);
    cliente_provisorio[*n_clientes_provisorios].tipo_cliente = *tipo_cliente;

    (*n_clientes_provisorios)++;
    linha_cont++;

    if (linha_cont >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero máximo de clientes provisórios!\n");
      return;
    }

    if (*n_clientes_provisorios >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero maximo de clientes provisórios!\n");
      return;
    }

  }
}

//funçao para alterar ficha cliente provisorio

void alterarFichClienteProv (ClienteProvisorio *listaClientesProvisorios, int *n_clientes_provisorios) {
  char nif[15];

  printf("Qual o NIF do cliente que pretende alterar: ");
  scanf("%s", nif);

  int indice = -1;

  for (int indiceClienteProvisorio = 0; indiceClienteProvisorio < *n_clientes_provisorios; indiceClienteProvisorio++) {
    if (strcmp(listaClientesProvisorios[indiceClienteProvisorio].nif, nif) == 0) {
      indice = indiceClienteProvisorio;
      break;
    }
  }

  if (indice == -1) {
    printf("Não existe nenhum cliente com esse NIF.\n");
    return;
  }

  int campo = -1;

  do {
    printf("Qual campo pretende alterar?\n");
    printf("1 - Nome\n");
    printf("2 - Morada\n");
    printf("3 - Código Postal\n");
    printf("4 - Localidade\n");
    printf("5 - NIF\n");
    printf("6 - Data de Criação\n");
    printf("7 - Tipo de Cliente\n");
    printf("0 - Sair\n");
    printf("Opção: ");
    scanf("%d", &campo);
  } while (campo < 0 || campo > 7);

  if (campo == 0) {
    return;
  }

  switch (campo) {
    case 1: {
      printf("Qual é o novo nome do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].nome);
      break;
    }
    
    case 2:  {
      printf("Qual é a nova morada do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].morada);
      break;
    }

    case 3: {
      printf("Qual é o novo código postal do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].codigo_postal);
      break;
    }

    case 4: {
      printf("Qual é a nova localidade do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].localidade);
      break;
    }

    case 5: {
      printf("Qual é o novo NIF do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].nif);
      break;
    }

    case 6: {
      printf("Qual é a nova data de criação da ficha do cliente: ");
      scanf("%s", listaClientesProvisorios[indice].data_criacao);
      break;
    }

    case 7: {
      printf("Qual é o novo tipo de cliente (N, V ou E): ");
      scanf(" %c", &listaClientesProvisorios[indice].tipo_cliente);
      break;
    }

  }

  printf("Ficha de cliente provisório alterada com sucesso!\n");
}

//eliminar ficha cliente provisorio

void eliminarFichClienteProv (ClienteProvisorio *listaClientesProvisorios, int *n_clientes_provisorios) {
  char nif[15];

  printf("Qual o NIF do cliente que pretende eliminar: ");
  scanf("%s", nif);

  int indice = -1;

  for (int indiceClienteProvisorio = 0; indiceClienteProvisorio < *n_clientes_provisorios; indiceClienteProvisorio++) {
    if (strcmp(listaClientesProvisorios[indiceClienteProvisorio].nif, nif) == 0) {
      indice = indiceClienteProvisorio;
      break;
    }
  }

  if (indice == -1) {
    printf("Não existe nenhum cliente com esse NIF.\n");
    return;
  }

  for (int indiceClienteProvisorio = indice; indiceClienteProvisorio < *n_clientes_provisorios - 1; indiceClienteProvisorio++) {
    listaClientesProvisorios[indiceClienteProvisorio] = listaClientesProvisorios[indiceClienteProvisorio + 1];
  }

  (*n_clientes_provisorios)--;

  printf("Ficha de cliente provisório eliminada com sucesso!\n");
}

//ler ficheiro precos base 

void lerFichPrecosBase (TabelaPrecosBase *precos_base, int *n_precos_base) {
  FILE *fich_precos_base = fopen("precobase.txt", "r");

  if (fich_precos_base == NULL) {
    printf("Não foi possível abrir o ficheiro de preços base!\n");
    return;
  }

  char buffer[BUFFER_SIZE];
  int linha_cont = 0; //contador de linhas

  // serve para ler a primeira linha de texto (cabeçalho) e ignorar
  fgets(buffer, BUFFER_SIZE, fich_precos_base);

  while (fgets(buffer, BUFFER_SIZE, fich_precos_base) != NULL && *n_precos_base < MAX_SERVICOS - 1) {
    char linha[BUFFER_SIZE];
    strcpy(linha, buffer);

    char *vista = strtok(linha, "\t");
    char *qualidade = strtok(NULL, "\t");
    char *max_pessoas = strtok(NULL, "\t");
    char *valor = strtok(NULL, "\t");
    char *valor_cancelamento = strtok(NULL, "\t");
    char *inicio_promocao = strtok(NULL, "\t");
    char *fim_promocao = strtok(NULL, "\t");
    char *valor_promocao = strtok(NULL, "\t");

    if (vista == NULL || qualidade == NULL || max_pessoas == NULL || valor == NULL || valor_cancelamento == NULL) {
      fprintf(stderr, "Erro na leitura dos campos do ficheiro de preços base\n");
      return;
    }

    precos_base[*n_precos_base].vista = *vista;
    precos_base[*n_precos_base].qualidade = *qualidade;
    precos_base[*n_precos_base].max_pessoas = *max_pessoas;
    precos_base[*n_precos_base].valor = atof(valor);
    precos_base[*n_precos_base].valor_cancelamento = atof(valor_cancelamento);

    if (inicio_promocao != NULL) {
      strcpy(precos_base[*n_precos_base].inicio_promocao, inicio_promocao);
      strcpy(precos_base[*n_precos_base].fim_promocao, fim_promocao);
      precos_base[*n_precos_base].valor_promocao = atof(valor_promocao);
    } else {
      // precos_base[*n_precos_base].inicio_promocao = NULL;
      // precos_base[*n_precos_base].fim_promocao = NULL;
      precos_base[*n_precos_base].valor_promocao = 0;
    }

    (*n_precos_base)++;
    linha_cont++;

    if (linha_cont >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero máximo de preços base!\n");
      return;
    }

    if (*n_precos_base >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero maximo de preços base!\n");
      return;
    }

  }
}

//ler ficheiro epoca 

void lerEpoca(TabelaAdicionalEpoca *adicional_epoca, int *n_epoca) {
  FILE *fich_epoca = fopen("epoca.txt", "r");

  if (fich_epoca == NULL) {
    printf("Não foi possível abrir o ficheiro de época!\n");
    return;
  }

  char buffer[BUFFER_SIZE];
  int linha_cont = 0; //contador de linhas

  // serve para ler a primeira linha de texto (cabeçalho) e ignorar
  fgets(buffer, BUFFER_SIZE, fich_epoca);

  while (fgets(buffer, BUFFER_SIZE, fich_epoca) != NULL && *n_epoca < MAX_SERVICOS - 1) {
    char linha[BUFFER_SIZE];
    strcpy(linha, buffer);

    char *TipoEpoca = strtok(linha, "\t");
    char *epoca_inicio = strtok(NULL, "\t");
    char *epoca_fim = strtok(NULL, "\t");
    char *percentagem_adicional = strtok(NULL, "\t");

    if (TipoEpoca == NULL || epoca_inicio == NULL || epoca_fim == NULL || percentagem_adicional == NULL) {
      fprintf(stderr, "Erro na leitura dos campos do ficheiro de época\n");
      return;
    }

    strcpy(adicional_epoca[*n_epoca].TipoEpoca, TipoEpoca);
    strcpy(adicional_epoca[*n_epoca].epoca_inicio, epoca_inicio);
    strcpy(adicional_epoca[*n_epoca].epoca_fim, epoca_fim);
    adicional_epoca[*n_epoca].percentagem_adicional = atof(percentagem_adicional);

    (*n_epoca)++;
    linha_cont++;

    if (linha_cont >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero máximo de épocas!\n");
      return;
    }

    if (*n_epoca >= MAX_SERVICOS) {
      fprintf(stderr, "Foi atingido o numero maximo de épocas!\n");
      return;
    }

  }
}

//transformar codigo do quarto em torre0andar0numero

char* transformarCodigoQuarto (int torre, int andar, int numero) {
  char *codigo = malloc(9 * sizeof(char));

  // {torre}0{andar}0{numero}
  sprintf(codigo, "%d0%d0%d", torre, andar, numero);

  return codigo;
}

void transformarCodigoQuartoInverso(char *codigo, int *torre, int *andar, int *numero) {
  char *token = strtok(codigo, "0");
  *torre = atoi(token);

  token = strtok(NULL, "0");
  *andar = atoi(token);

  token = strtok(NULL, "0");
  *numero = atoi(token);
}

//verificar se nif da ficha provisoria existe na ficha cliente

int verificarNIFCliente (ClienteProvisorio *listaClientesProvisorios, int n_clientes_provisorios, char nif[15]) {
  for (int indiceClienteProvisorio = 0; indiceClienteProvisorio < n_clientes_provisorios; indiceClienteProvisorio++) {
    if (strcmp(listaClientesProvisorios[indiceClienteProvisorio].nif, nif) == 0) {
      return 1;
    }
  }

  return 0;
}

int calcularNumeroDeDiasEntreDatas(char *data_checkin, char *data_checkout) {
  int dia_checkin = atoi(strtok(data_checkin, "/"));
  int mes_checkin = atoi(strtok(NULL, "/"));
  int ano_checkin = atoi(strtok(NULL, "/"));

  int dia_checkout = atoi(strtok(data_checkout, "/"));
  int mes_checkout = atoi(strtok(NULL, "/"));
  int ano_checkout = atoi(strtok(NULL, "/"));

  int dias = 0;

  while (dia_checkin != dia_checkout || mes_checkin != mes_checkout || ano_checkin != ano_checkout) {
    dias++;
    dia_checkin++;

    if (dia_checkin > 31) {
      dia_checkin = 1;
      mes_checkin++;
    }

    if (mes_checkin > 12) {
      mes_checkin = 1;
      ano_checkin++;
    }
  }

  return dias; 
}

void parse_data_DD_MM_YYYY(char *parseData, int *dia, int *mes, int *ano) {
  *dia = atoi(strtok(parseData, "/"));
  *mes = atoi(strtok(NULL, "/"));
  *ano = atoi(strtok(NULL, "/"));
}

void parse_data_MM_DD(char *parseData, int *mes, int *dia) {
  *mes = atoi(strtok(parseData, "/"));
  *dia = atoi(strtok(NULL, "/"));
}


float calcularValorTotalReserva(TabelaPrecosBase *precos_base, int n_precos_base, TabelaAdicionalEpoca *adicional_epoca, int n_epoca, char vista, char qualidade, char max_pessoas, char data_checkin[11], char data_checkout[11], int num_pessoas) {
  float valor_total = 0;

  int dia_checkin, mes_checkin, ano_checkin, dia_checkout, mes_checkout, ano_checkout;

  parse_data_DD_MM_YYYY(data_checkin, &dia_checkin, &mes_checkin, &ano_checkin);
  parse_data_DD_MM_YYYY(data_checkout, &dia_checkout, &mes_checkout, &ano_checkout);

  int encontrado_base = 0;
  for (int indicePrecoBase = 0; indicePrecoBase < n_precos_base; indicePrecoBase++) {
    if (precos_base[indicePrecoBase].vista == vista && precos_base[indicePrecoBase].qualidade == qualidade && precos_base[indicePrecoBase].max_pessoas == max_pessoas) {
      int dias = calcularNumeroDeDiasEntreDatas(data_checkin, data_checkout);
      valor_total = precos_base[indicePrecoBase].valor * dias;

      if (precos_base[indicePrecoBase].inicio_promocao != NULL) {
        int dia_inicio_promocao, mes_inicio_promocao, dia_fim_promocao, mes_fim_promocao;
        parse_data_MM_DD(precos_base[indicePrecoBase].inicio_promocao, &mes_inicio_promocao, &dia_inicio_promocao);
        parse_data_MM_DD(precos_base[indicePrecoBase].fim_promocao, &mes_fim_promocao, &dia_fim_promocao);

        if (
          mes_checkin >= mes_inicio_promocao && mes_checkout <= mes_fim_promocao
          && (
            (mes_checkin == mes_inicio_promocao && dia_checkin >= dia_inicio_promocao)
            || (mes_checkout == mes_fim_promocao && dia_checkout <= dia_fim_promocao)
          )
        ) {
          // descontar valor em epocas especiais
          valor_total -= valor_total * precos_base[indicePrecoBase].valor_promocao/100;
        }
      }

      encontrado_base = 1;
      break;
    }
  }

  if (encontrado_base == 0) {
    printf("Não existe nenhum preço base para o quarto com essas características.\n");
    return 0;
  }

  for (int indiceEpoca = 0; indiceEpoca < n_epoca; indiceEpoca++) {
    int dia_inicio_epoca, mes_inicio_epoca, dia_fim_epoca, mes_fim_epoca;
    parse_data_MM_DD(adicional_epoca[indiceEpoca].epoca_inicio, &mes_inicio_epoca, &dia_inicio_epoca);
    parse_data_MM_DD(adicional_epoca[indiceEpoca].epoca_fim, &mes_fim_epoca, &dia_fim_epoca);

    if (
      mes_checkin >= mes_inicio_epoca && mes_checkout <= mes_fim_epoca
      && (
        (mes_checkin == mes_inicio_epoca && dia_checkin >= dia_inicio_epoca)
        || (mes_checkout == mes_fim_epoca && dia_checkout <= dia_fim_epoca)
      )
    ) {
      // adicionar valor em epocas especiais
      valor_total += valor_total * (adicional_epoca[indiceEpoca].percentagem_adicional / 100);
    }
  }

  return valor_total;
}

//verificar reserva que vai incluir quartos , tabela preco base , tabela adicional epoca e ficha cliente provisoria

void criarReserva(Reserva *reservas, int *n_reservas, int *num_reserva, Quarto *quartos, int n_quartos, TabelaPrecosBase *precos_base, int n_precos_base, TabelaAdicionalEpoca *adicional_epoca, int n_epoca, ClienteProvisorio *clientes_provisorios, int n_clientes_provisorios) {

  //perguntar por ficha cliente provisoria
  printf("Qual o NIF do cliente provisório: ");
  char nif[15];
  scanf("%s", nif);

  if (verificarNIFCliente(clientes_provisorios, n_clientes_provisorios, nif) == 0) {
    printf("Não existe nenhum cliente provisório com esse NIF. A criar um novo cliente provisório!\n");
    criarFichaClienteProvisorio(clientes_provisorios, &n_clientes_provisorios);
    return;
  }

  //perguntar por torre andar e quarto
  int torre, andar, numero;

  printf("Qual é a torre do quarto (1 ou 2): ");
  scanf("%d", &torre);

  if (torre < 1 || torre > 2) {
    printf("Entrada inválida para torre.\n");
    return;
  }

  printf("Qual é o andar do quarto (1 a 10): ");
  scanf("%d", &andar);

  if (andar < 1 || andar > 10) {
    printf("Entrada inválida para andar.\n");
    return;
  }

  printf("Qual é o número do quarto (1 a 200): ");
  scanf("%d", &numero);

  if (numero < 1 || numero > 200) {
    printf("Entrada inválida para número do quarto.\n");
    return;
  }

  char vista;
  char qualidade;
  char max_pessoas;

  int encontrado=0;
  //loop para percorrer quartos
  for(int i = 0; i < n_quartos; i++) {
    if (quartos[i].torre == torre && quartos[i].andar == andar && quartos[i].numero == numero) {
      vista = quartos[i].vista;
      qualidade = quartos[i].qualidade;
      max_pessoas = quartos[i].max_pessoas;

      encontrado = 1;
      if (quartos[i].disponibilidade == 0) {
        printf("O quarto não está disponível.\n");
        return;
      }

      break;
    }
  }

  if (encontrado == 0) {
    printf("Não existe nenhum quarto com esses dados.\n");
    return;
  }

  Reserva reserva;

  (*num_reserva)++;
  reserva.num_reserva = *num_reserva;

  strcpy(reserva.quarto_codigo, transformarCodigoQuarto(torre, andar, numero));

  //data check in
  int dia, mes, ano;
  printf("Qual a data de check-in (dd/mm/aaaa): ");
  scanf("%d/%d/%d", &dia, &mes, &ano);

  sprintf(reserva.data_checkin, "%02d/%02d/%04d", dia, mes, ano);

  //data check out
  int dia_checkout, mes_checkout, ano_checkout;
  printf("Qual a data de check-out (dd/mm/aaaa): ");
  scanf("%d/%d/%d", &dia_checkout, &mes_checkout, &ano_checkout);
  
  sprintf(reserva.data_checkout, "%02d/%02d/%04d", dia_checkout, mes_checkout, ano_checkout);

  //calcular valor tendo em conta preco base e adicional epoca

  int torreQuarto, andarQuarto, numeroQuarto;
  transformarCodigoQuartoInverso(reserva.quarto_codigo, &torreQuarto, &andarQuarto, &numeroQuarto);



  
  printf("Qual o número de pessoas: ");
  scanf("%d", &reserva.num_pessoas);
  
  /* printf("Qual o número de quartos adicionais: ");
  scanf("%d", &reserva.quartos_adicionais)*/

  reserva.valor_total = calcularValorTotalReserva(precos_base, n_precos_base, adicional_epoca, n_epoca, vista, qualidade, max_pessoas, reserva.data_checkin, reserva.data_checkout, reserva.num_pessoas);

  printf("Qual o valor adiantado (total: %.02f): ", reserva.valor_total);
  scanf("%f", &reserva.valor_pago);
 
  if (reserva.valor_pago < reserva.valor_total / 3) {
    printf("O valor adiantado tem de ser pelo menos 1/3 do valor total.\n");
    return;
  }

  printf("Qual a forma de pagamento: ");
  scanf("%s", reserva.forma_pagamento);

  printf("Qual a situação da reserva (P, C, O ou F): ");
  scanf(" %c", &reserva.situacao_reserva);

  printf("Qual a observação: ");
  scanf(" %s", reserva.observacoes);


//mudar estado do quarto 

  // for (int i = 0; i < n_quartos; i++) {
  //   if (quartos[i].torre == torre && quartos[i].andar == andar && quartos[i].numero == numero) {
  //     quartos[i].disponibilidade = 0;
  //     break;
  //   }
  // }

  //printar reserva com sucesso e a frente mostrar o numero de reserva

  printf("Reserva criada com sucesso!\n");
  printf("Número de Reserva: %d\n", reserva.num_reserva);

  reservas[*n_reservas] = reserva;
  (*n_reservas)++;  
}
//escrever ficheiro binario reservas

void lerFichReservas(Reserva *reservas, int *n_reservas, int *num_reserva) {
  FILE *fich_reservas = fopen("reservas.dat", "rb");

  if (fich_reservas == NULL) {
    printf("Não foi possível abrir o ficheiro de reservas!\n");
    return;
  }

  fread(n_reservas, sizeof(int), 1, fich_reservas);

  printf("Numero de reservas: %d\n", *n_reservas);

  for (int i = 0; i < *n_reservas; i++) {
    fread(&reservas[i], sizeof(Reserva), 1, fich_reservas);
  }

  (*num_reserva) = reservas[*n_reservas - 1].num_reserva;

  fclose(fich_reservas);
}


void escreverFichReservas(Reserva reservas[], int n_reservas) {
  FILE *fich_reservas = fopen("reservas.dat", "wb+");

  if (fich_reservas == NULL) {
    printf("Não foi possível abrir o ficheiro de reservas!\n");
    return;
  }

  fwrite(&n_reservas, sizeof(int), 1, fich_reservas);

  for (int i = 0; i < n_reservas; i++) {
    fwrite(&reservas[i], sizeof(Reserva), 1, fich_reservas);
  }

  fclose(fich_reservas);
}

//listar reservas

void listarReservas(Reserva *reservas, int n_reservas) {
  for (int indiceReserva = 0; indiceReserva < n_reservas; indiceReserva++) {
    printf("Número de Reserva: %d\n", reservas[indiceReserva].num_reserva);
    printf("Código do Quarto: %s\n", reservas[indiceReserva].quarto_codigo);
    printf("Data de Check-in: %s\n", reservas[indiceReserva].data_checkin);
    printf("Data de Check-out: %s\n", reservas[indiceReserva].data_checkout);
    printf("Número de Pessoas: %d\n", reservas[indiceReserva].num_pessoas);
    printf("Valor Total: %.2f\n", reservas[indiceReserva].valor_total);
    printf("Valor Pago: %.2f\n", reservas[indiceReserva].valor_pago);
    printf("Forma de Pagamento: %s\n", reservas[indiceReserva].forma_pagamento);
    printf("Situação da Reserva: %c\n", reservas[indiceReserva].situacao_reserva);
    printf("Observações: %s\n", reservas[indiceReserva].observacoes);
    printf("\n");
    printf("--------------------------------\n");
  }
}

  


int main() 
{
  setlocale(LC_ALL, "Portuguese");
  int num_reserva = 0; // inicializamos o número de reservas que vão existir no total
  
  TabelaServicos servicos[500];
  int numServico = 0;
  
  Quarto quartos[2000];
  int numQuartos = 0;
  
  ClienteProvisorio clientes_provisorios[1000];
  int n_clientes_provisorios = 0;
  
  TabelaPrecosBase precos_base[16];
  int n_precos_base = 0;
  
  TabelaAdicionalEpoca adicional_epoca[4];
  int n_epoca = 0;
  
  Reserva reservas[1000];
  int n_reservas = 0;


  // Leitura inicial
  lerFichQuartos(quartos, &numQuartos);
  lerFichServicos(servicos, &numServico);
  lerFichClientesProv(clientes_provisorios, &n_clientes_provisorios);
  lerFichPrecosBase(precos_base, &n_precos_base);
  lerEpoca(adicional_epoca, &n_epoca);
  lerFichReservas(reservas, &n_reservas, &num_reserva);
/*
  /*for (int i = 0; i < numServico; i++)
  {
    printf("%s\t%.2f\n", servicos[i].tipo_servico, servicos[i].valor);
  }
*/

  // Lógica

  //prints teste
/*
  for (int i = 0; i < numQuartos; i++)
  {
    printf("%d\t%d\t%d\t%c\t%c\t%c\n", quartos[i].torre, quartos[i].andar, quartos[i].numero, quartos[i].vista, quartos[i].qualidade, quartos[i].max_pessoas);
  }


  // print reservas
  printf("Reservas:\n");
  for (int i = 0; i < n_reservas; i++)
  {
    printf("%d\t%s\t%s\t%s\t%d\t%.2f\t%.2f\t%s\t%c\t%s\n", reservas[i].num_reserva, reservas[i].quarto_codigo, reservas[i].data_checkin, reservas[i].data_checkout, reservas[i].num_pessoas, reservas[i].valor_total, reservas[i].valor_pago, reservas[i].forma_pagamento, reservas[i].situacao_reserva, reservas[i].observacoes);
  }

   //alterarServico(servicos, &numServico);
  //alterarQuarto(quartos, numQuartos);

  /*for (int i = 0; i < numQuartos; i++)
  {
    printf("%d\t%d\t%d\t%c\t%c\t%c\n", quartos[i].torre, quartos[i].andar, quartos[i].numero, quartos[i].vista, quartos[i].qualidade, quartos[i].max_pessoas);
  }
*/
  //registrarQuarto(quartos, &numQuartos);  
  //registarServico(servicos, &numServico);
  //removerServico(servicos, &numServico);
  //criarFichaClienteProvisorio(clientes_provisorios, &n_clientes_provisorios);
  //alterarFichClienteProv(clientes_provisorios, &n_clientes_provisorios);
  //eliminarFichClienteProv(clientes_provisorios, &n_clientes_provisorios);
  criarReserva(reservas,&n_reservas ,&num_reserva, quartos, numQuartos, precos_base, n_precos_base, adicional_epoca, n_epoca, clientes_provisorios, n_clientes_provisorios);
  //listarReservas(reservas, n_reservas);


  //menu separado em submenus 

  escreverFichQuartos(quartos, numQuartos);
  escreverFichServicos(servicos, numServico);
  escreverFichClienteProv(clientes_provisorios, n_clientes_provisorios);
  escreverFichReservas(reservas, n_reservas);
}


  





  
  
     

