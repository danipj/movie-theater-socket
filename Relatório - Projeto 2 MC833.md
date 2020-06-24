# Relatório - Projeto 2 MC833

Daniela Palumbo - 166301

Felipe Vieira - 155335

## Introdução

O objetivo do projeto é estruturar um sistema de cinema, com cadastro de filmes
e alocação de salas, baseado na comunicação UDP entre cliente e servidor. Deve ser possível realizar o cadastro de filmes, consulta por salas, títulos, ids, listagem de todos os filmes, dentre outros. A partir da comunicação entre cliente e servidor utilizando o protocolo UDP, é possível enviar uma opção ao servidor a partir do cliente, inserir ou recuperar os dados desejados e, apenas ao final, encerrar o programa. O servidor se encarrega de ouvir as opções desejadas pelo cliente e devolver os dados quando necessário. 

## Descrição geral e casos de uso

O projeto constitui de um cliente e um servidor que se comunicam atraveś do protocolo UDP. No cliente temos uma visualização de um menu e das opções que podem ser realizadas: cadastro de um filme, consulta de um filme por seu ID, consulta de um título de filme por seu ID, consulta de filmes por gênero, consulta de títulos de filmes e suas salas, consulta de todos os filmes e remoção de filmes, dando também a opção de encerrar o programa. O servidor, por sua vez, analisa qual foi a opção enviada pelo cliente, realiza a ação esperada, consultando o banco de dados, e retorna o resultado para o cliente.

|  função                                                                                                              | descrição                                                                                     |
|----------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| int   create_movie ( int   id ,  char   title [ 50 ],  char   genre [ 50 ],  int   room ,  char   synopsis [ 200 ]); | Adiciona um filme no banco de dados. Aceita os parâmetros id, título, gênero, sala e sinopse. |
| int   delete_movie ( char   movie_id );                                                                              | Deleta um filme do banco de dados. Aceita o id do filme como parâmetro.                       |
| int   list_movie_title_rooms ( char   * response );                                                                  | Lista todos os títulos e salas de filmes.                                                     |
| int list_movie_by_gender(char genre[50], char * response);                                                                  | Lista todos os filmes de determinado gênero, passado por parâmetro.                                                     |
| int find_title_by_id(char id, char * response);                                                                  | Dado um id, devolve o título do filme associado.                                                     |
| int find_info_by_id(char id, char * response);                                                                  | Dado um id, devolve todas as informações do filme associado.                                                     |
| int list_all(char * response);                                                                  | Lista todos os filmes.                                                     |

O cliente deve, portanto, inserir os filmes desejados no banco de dados, utilizando a função ```create_movie```. Uma vez inseridos, é possível realizar quaisquer consultas, bem como apagar o filme do banco.

## Armazenamento e estrutura de dados

Como armazenamento escolhemos operar um arquivo de texto, por considerar ser de menor complexidade do que um banco de dados elaborado. Assim, armazenamos a estrutura do filme, um campo por linha, apenas escrevendo no arquivo. Dessa maneira pudemos resgatar os ids do filme lendo a cada 5 linhas, por exemplo.

Utilizamos a estrutura `movie`, limitando em alguns caracteres os campos de string para ser de mais fácil manipulação, como a seguir:
```
typedef struct movie
{
    int id;
    char title[50];
    char genre[50];
    int room;
    char synopsis[200];
} movie;
```

Realizamos alguns testes para avaliar se o nosso método seria eficaz o suficiente, como mostrado abaixo:
### 200 Execuções de Inserção
| média | mediana | desvio padrão | max | min |
| ------|---------|---------------|-----|-----|
|0.00004|0.00004  |0.00002        |0.00015|0.00002|

![alt text](https://github.com/danipj/movie-theater-socket/blob/master/insert_plot.png "Gráfico de tempos de inserção")


### 200 Execuções de Listagem
| média | mediana | desvio padrão | max | min |
| ------|---------|---------------|-----|-----|
|0.00007|0.00006  |0.00003        |0.00024|0.00003|

![alt text](https://github.com/danipj/movie-theater-socket/blob/master/list_plot.png "Gráfico de tempos de listagem")

Assim, decidimos manter essa implementação, sem necessidade de adicionar complexidade de banco de dados ao código.

## Detalhes de implementação
A comunicação cliente-servidor é feita através da biblioteca nativa de sockets.
O servidor abre um socket e usa de um spin-lock para aguardar por clientes. Quando
uma conexão chega, ele cria um fork para atender o pedido e ouve pelo pedido
do cliente.

O cliente, por sua vez, abre também um soquete e tenta se comunicar com o servidor.
Uma vez que a conexão é aceita, ele envia a operação selecionada juntamente dos
possíveis dados necessários. Caso seja necessário, ele aguarda por dados
adicionais do servidor.

Quando o servidor recebe os dados do cliente, ele seleciona a operação apropriada
e, caso necessário, envia mais dados de volta. Caso alguma das funções apresente
um erro (como não ter achado o arquivo dos dados, por exemplo), o servidor devolve o código -1.

Adicionalmente, implementamos um sistema de teste em Python. De acordo com um
número de interações configurável, o sistema invoca o cliente diversas vezes,
agrega os tempos de resposta conforme reportados pelo cliente e computa um
sumário estatístico dos tempos de requisição. O tempo medido é obtido pelo
cliente ao invés de, por exemplo, ser calculado de dentro da chamada do
python para evitar erros de medida vindo do overhead que o sistema
de teste pode ter ou de eventuais interrupções do SO que o tester
em si pode receber.

## Diferenças entre o protocolo TCP e UDP

Ao contrário do implementado anteriormente no projeto 1, em que foi utilizado o TCP, utilizando o UDP são enviados datagramas inteiros através da rede, não é necessário criar forks mas devemos implementar select ou poll. 

Uma vantagem de se usar o UDP é que temos uma comunicação mais rápida. Por causa da sua velocidade, o protocolo UDP é interessante para streaming de vídeos, vídeochamadas e jogos online. Tais operações acabam não sendo muito prejudicadas caso algum pacote se perca no meio do caminho, uma vez que não no UDP não existe nenhum _handshake_ nem confirmação de que os datagramas foram enviados com sucesso. Pode ser que seja necessário reordenar os pacotes, algo que não acontece com o TCP, que além de confirmar que os pacotes chegaram, captura os pacotes sequencialmente.

Também é necessário uma leitura bloqueante então fica difícil realizar qualquer outra ação em paralelo.

## Conclusão

Concluímos a partir desse projeto que a comunicação UDP é rápida, mas por ser propensa a perda de pacotes é necessário trabalhar outras maneiras de garantir que os dados estão chegando a seu destino correto. Logo, para um sistema de cinema em que haveriam múltiplos acessos simultâneos e seria necessária uma garantia na comunicação, afinal uma suposta venda de ingressos poderia ser o objetivo final, o protocolo UDP não é o ideal.
