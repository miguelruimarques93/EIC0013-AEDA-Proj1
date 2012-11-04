Gestão de jobs em "Grid Computing"
==================================

- **Duarte Duarte** - 110509101 - ei11101@fe.up.pt
- **Hugo Freixo** - 110509086 - ei11086@fe.up.pt
- **Miguel Marques** - 110509099 - ei11099@fe.up.pt

3 de Novembro de 2012
*Turma 1 - 2MIEIC - FEUP*

O Grid Manager
--------------

O **Grid Manager** é um gestor de uma rede de computação composta por várias máquinas de diferentes
características e de vários utilizadores que podem ser utilizadores académicos ou utilizadores de empresas.
Estes utilizadores podem criar novos trabalhos (*jobs*) que serão executados num determinado tempo
especificado pelo utilizador que o criou.


Solução Implementada
--------------------

O grupo de trabalho optou por implementar não só um gestor como também um simulador.
Estando tudo integrado na mesma aplicação à medida que se vão adicionando mais trabalhos, máquinas e/ou utilizadores o gestor vai atualizando os trabalhos, isto é, tendo em conta o tempo especificado aquando da criação de um novo trabalho o simulador irá apagar esse trabalho quando esse tempo tiver passado, tempo esse que é medido em segundos e em tempo real.

Para o gestor foi implementada uma estrutura de dados em que todas as classes são derivadas de interfaces (classes puras) que definem algumas funcionalidades básicas do gestor. São elas: 
- **IPrint** - Imprimir para uma stream os dados de um objeto;
- **ISave** - Guardar num ficheiro binário os dados de um objeto.

Para o simulador foram implementadas duas interfaces:
- **IUpdate** - Atualizar os dados de um objeto com base numa diferença temporal;
- **Runnable** - Objeto que pode executar algum trabalho num *thread* separado.

O grupo teve implementou um logger com recurso a um singlenton e um policy based design para manter o utilizador atualizado acerca do sistema, bem como para fazer debuging de algumas features implementadas.
Para gravar os dados da sessão optou-se pela utilização de ficheiros binários, visto que são mais compactos e menos propensos a erros de leitura, e para a sua leitura e escrita implementou-se uma classe (ByteBuffer) que lê e escreve para um determinado ficheiro diversos tipos de dados.
A interface do utilizador, toda ela feita através da consola, foi maioritariamente feita com recurso a uma feature implementada através de menus (classes IMenu, Menu e Menu::Item) lidos de ficheiros de texto e que após o utilizador selecionar um Menu::Item retornam um valor inteiro que permite gerir a sua opção.
No decurso do desenvolvimento deste trabalho recorremos a features do c++11, nomeadamente:
- Lambda functions and expressions (funções anónimas);
- Type inference;
- Explicit overrides;
- Regular Expressions;
- Thread implementation of the c++ standard library;
- Tuples;
- Ranged-based for loops


Diagrama de classes (UML)
-------------------------

...


Casos de Utilização
--------------------

- Adicionar utilizadores ao sistema
- Remover utilizadores do sistema
- Alterar nome dos utilizadores
- Alterar orçamento dos utilizadores empresariais
- Procurar utilizadores no sistema através do nome
- Listar todos os utilizadores ou apenas utilizadores académicos ou empresariais
- Adicionar máquinas ao sistema
- Remover máquinas do sistema
- Alterar nome das máquinas
- Alterar RAM das máquinas
- Alterar espaço do disco das máquinas
- Adicionar Software às máquinas
- Remover Software das máquinas
- Listar Software instalado numa máquina
- Alterar o número máximo de trabalhos de uma máquina
- Listar os trabalhos em execução numa máquina
- Remover um trabalho de uma máquina
- Remover todos os trabalhos de uma máquina
- Listar o Software necessário de um trabalho
- Listar o Software disponível numa máquina
- Procurar máquinas no sistema através do nome, RAM disponível, espaço do disco disponível ou número de trabalhos
- Listar todas as máquinas
- Criar novos trabalhos
- Procurar trabalhos no sistema através do nome, RAM necessária, espaço do disco necessário, prioridade ou tempo de execução
- Listar todos os trabalhos do sistema


Dificuldades
------------

Não houve grandes dificuldades no planeamento nem na implementação do trabalho com exceção de alguns entraves na especialização de funções genéricas (com recurso a *templates*) e na execução de funções quando se fecha a consola abruptamente, casos que nos deram mais trabalho a resolver.


Distribuição do trabalho pelos elementos do grupo
-------------------------------------------------

...

