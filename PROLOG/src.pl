
% ***************************************************************************
% *                              Bibliotecas                                *                      
% ***************************************************************************
:-use_module(library(random)).
:-use_module(library(sockets)).
port(60070).

% ***************************************************************************
% *                            Implementao                                *                      
% ***************************************************************************
estado_inicial([[1,1,1,1,1],
                [0,0,0,0,0],
                [0,0,3,0,0],
                [0,0,0,0,0],
                [2,2,2,2,2]]).

inicio:-
        apresentacao,
        menu_inicio(J1, J2),
        estado_inicial(Tab),
        neutrao_inicial(N),
        visualiza_estado(Tab),
        primeira_jogada(Tab, N, 1, J1, J2).

neutrao_inicial([2,2]).

menu_inicio(J1i, J2i):-
        write('Tipo de Jogador 1 (0 humano, 1 computador RANDOM, 2 computador INTELIGENTE): '),
        nl,
        get_code(J1),
        get_char(_),
        write('Tipo de Jogador 2 (0 humano, 1 computador RANDOM, 2 computador INTELIGENTE):'),
        nl,
        get_code(J2),
        get_char(_),
        J1i is J1 - 48,
        J2i is J2 - 48.

apresentacao:-
        write('------------------NEUTRON------------------'), nl,
        write('-   Jogo desenvolvido na cadeira de PLOG  -'), nl,
        write('-                                         -'), nl,
        write('- Objetivo:                               -'), nl,
        write('-    Colocar o Neutro numa das posies  -'), nl,
        write('-    iniciais dos seus soldados.          -'), nl,
        write('-                                         -'), nl,
        write('- 2 Jogadores->                           -'), nl,
        write('-    - Peas pretas                       -'), nl,
        write('-    - Peas brancas                      -'), nl,
        write('-                                         -'), nl,
        write('- Escolha o tipo de jogadores:            -'), nl,
        nl.

visualiza_estado(Tab):-
        nl,
        write('  '),
        colunas(0), nl,
        print_tab(0, Tab),
        nl,!.

jogada_soldado(Tab,Xi, Yi, Xf, Yf, J):-
        (
           ler_jogada_soldado(Xi, Yi, Xf, Yf),
           valida_jogada(Tab, Xi, Yi, Xf, Yf,J)
        ;
           write('Jogada Invlida!\n'),
           jogada_soldado(Tab,Xi, Yi, Xf, Yf,J)
        ).

jogada_neutrao(Tab,NXi, NYi, NXf, NYf):-
        (
           ler_jogada_neutrao(NXf, NYf),
           write('\nLeu Jogada\n'),
           valida_jogada(Tab, NXi, NYi, NXf, NYf,3)
        ;
           write('Jogada Invlida!\n'),
           jogada_neutrao(Tab, NXi, NYi, NXf, NYf)
        ).
        
primeira_jogada(Tab, [NX,NY], J, J1, J2):-
        (
           J1 = 0,
           jogada_soldado(Tab, Xi, Yi, Xf, Yf, J)
        ;
           jogada_aleatoria(Tab, Xi, Yi, Xf, Yf, J)
         ),
        atualiza_jogada(Tab, Xi, Yi, Xf, Yf, Tab_f2),
        visualiza_estado(Tab_f2),
        !,
        (
           verifica_fim(NX, NY, Tab_f2, J)
        ;
           (
                 J is 1,
                 jogo(Tab_f2, [NX,NY], 2, J2, J1);
                 jogo(Tab_f2, [NX,NY], 1, J2, J1)
              
           )
        ). 

jogo(Tab,[NX,NY], J, J1, J2):-
        (
           J1 = 0,
           jogada_neutrao(Tab, NX, NY, NXf, NYf)
        ;
           J1 = 1,
           jogada_aleatoria(Tab, NX, NY, NXf, NYf, 3)
        ;
           jogada_inteligente_neutron(Tab, NX, NY, NXf, NYf, J)
        ),
        atualiza_jogada(Tab, NX, NY, NXf, NYf, Tab_f2),
        visualiza_estado(Tab_f2),
        !,
        (
           verifica_fim(NXf, NYf, Tab_f2, J)
        ;
           (
              J1 = 0,
              jogada_soldado(Tab_f2, Xi, Yi, Xf, Yf, J)
           ;
              jogada_aleatoria(Tab_f2, Xi, Yi, Xf, Yf, J)),
           atualiza_jogada(Tab_f2, Xi, Yi, Xf, Yf, Tab_f3),
           visualiza_estado(Tab_f3),
           !,
           (
              verifica_fim(NXf, NYf, Tab_f3, J);
              (
                 J is 1,
                 jogo(Tab_f3, [NXf,NYf], 2, J2, J1);
                 jogo(Tab_f3, [NXf,NYf], 1, J2, J1)
              )
           )
        ). 

colunas(4) :- write(4), !, nl, write(' ---------------').

colunas(X) :- 
        write(X),
        write('  '),
        X2 is X + 1,
        colunas(X2).

escreve(0):-write('| |').
escreve(1):-write('|'),put_code(9679),write('|').
escreve(2):-write('|'),put_code(9676),write('|').
escreve(3):-write('|'),put_code(9678),write('|').

print_tab(_,[]).
print_tab(N,[Linha|Resto]):-
        write(N),
        print_linha(Linha),
        write(' ---------------'), nl,
        N2 is N+1,
        print_tab(N2, Resto).

print_linha([]) :- nl.
print_linha([Elemento|Resto]):-
        escreve(Elemento),
        print_linha(Resto).

ler_jogada_soldado(Xi, Yi, Xf, Yf):-
        write('Inserir coordenada X inicial da pea do soldado a mover'),
        nl,
        get_code(X1),
        get_char(_),
        write('Inserir coordenada Y inicial da pea do soldado a mover'),
        nl,
        get_code(Y1),
        get_char(_),
        write('Inserir coordenada X final da pea do soldado a mover'),
        nl,
        get_code(X2),
        get_char(_),
        write('Inserir coordenada Y final da pea do soldado a mover'),
        nl,
        get_code(Y2),
        get_char(_),
        % 48 cdigo tecla zero
        Xi is X1 - 48,
        Yi is Y1 - 48,
        Xf is X2 - 48,
        Yf is Y2 - 48.

ler_jogada_neutrao(NXf,NYf):-
        write('Inserir coordenada X final da pea neutro, a mover'),
        nl,
        get_code(X),
        get_char(_),
        write('Inserir coordenada Y final da pea neutro, a mover'),
        nl,
        get_code(Y),
        get_char(_),
        NXf is X - 48,
        NYf is Y - 48.

atualiza_jogada(Tab, Xi, Yi, Xf, Yf, Tab_f2):-
        busca_elemento(Tab, Xi, Yi, E),
        muda_elemento(Tab, Xf, Yf, E, Tab_f),
        muda_elemento(Tab_f, Xi, Yi, 0, Tab_f2).

busca_elemento([Tab|_], 0, Yi, E):-
        busca_linha(Tab, Yi, E).

busca_elemento([_|Tab], Xi, Yi, E):-
        X is Xi - 1,
        busca_elemento(Tab, X, Yi, E).

busca_linha([Tab|_], 0, Tab).

busca_linha([_|Tab], Yi, E):-
        Y is Yi - 1,
        busca_linha(Tab, Y, E).

muda_elemento([Tab|Tail], 0, Yf, E, [Tab_f|Tail]):-
        muda_linha(Tab, Yf, E, Tab_f).

muda_elemento([Tab|Tail], Xf, Yf, E, [Tab|Tab_f]):-
        X is Xf - 1,
        muda_elemento(Tail, X, Yf, E, Tab_f).

muda_linha([_|Tail], 0, E, [E|Tail]).

muda_linha([Tab|Tail], Yf, E, [Tab|Tail_f]):-
        Y is Yf - 1,
        muda_linha(Tail, Y, E, Tail_f).

verifica_fim(Nx,Ny,Tab,J):-
        (
           (
              Nx == 0;
              J == 2,
              verifica_encurralado(Tab, Nx, Ny),
              write('cenas')
           ), write('------------------Jogador Preto Ganhou!---------------'), 
              print_fim_de_jogo
        ; 
            (
              Nx == 4;
              J == 1,
              verifica_encurralado(Tab, Nx, Ny),
              write('cenas')
           ), write('------------------Jogador Branco Ganhou!---------------'), 
              print_fim_de_jogo
        ).

valida_jogada(Tab, Xi, Yi, Xf, Yf, N):-
        write([Tab, Xi, Yi, Xf, Yf, N]),
        (Xi =\= Xf;
        Yi =\= Yf),
        
        busca_elemento(Tab, Xi, Yi, N),
        (
           Xf = Xi,
           (
              Yf > Yi,
              M is 2;
              M is 6
           )
        ;
           Yf = Yi,
           (
              Xf > Xi,
              M is 4;
              M is 0
           )
        ;
           DX is Xf - Xi, 
           DY is Yf - Yi,
           Adx is abs(DX),
           Ady is abs(DY),
           Adx = Ady,
           (
              DX < 0, DY > 0,
              M is 1;
              DX > 0, DY > 0,
              M is 3;
              DX > 0, DY < 0,
              M is 5;
              DX < 0, DY < 0,
              M is 7
           )
        ),
        verifica_maximo(Tab, Xi, Yi, Xm, Ym, M,N),
        !,
        %write('\nValores '),write(Xm),nl,write(Ym),nl,nl,
        Xm = Xf,
        Ym = Yf.

verifica_maximo(Tab, Xi, Yi, Xm, Ym, M, N):-
       busca_elemento(Tab, Xi, Yi, N),
       (
          M = 0,
          X1 is Xi - 1,
          Y1 is Yi;
          M = 1,
          X1 is Xi - 1,
          Y1 is Yi + 1;         
          M = 2,
          X1 is Xi,
          Y1 is Yi + 1;         
          M = 3,
          X1 is Xi + 1,
          Y1 is Yi + 1;         
          M = 4,
          X1 is Xi + 1,
          Y1 is Yi;         
          M = 5,
          X1 is Xi + 1,
          Y1 is Yi - 1;         
          M = 6,
          X1 is Xi,
          Y1 is Yi - 1;         
          M = 7,
          X1 is Xi - 1,
          Y1 is Yi - 1
       ),
       (
          verifica_maximo(Tab, X1, Y1, Xm, Ym, M, 0);
          Xm is Xi,
          Ym is Yi
       ).
        
verifica_encurralado(Tab, NX, NY):-
        verifica_maximo(Tab, NX, NY, X0, Y0, 0, 3),
        !,
        NX == X0,
        NY == Y0,
        verifica_maximo(Tab, NX, NY, X1, Y1, 1, 3),
        !,
        NX == X1,
        NY == Y1,
        verifica_maximo(Tab, NX, NY, X2, Y2, 2, 3),
        !,
        NX == X2,
        NY == Y2,
        verifica_maximo(Tab, NX, NY, X3, Y3, 3, 3),
        !,
        NX == X3,
        NY == Y3,
        verifica_maximo(Tab, NX, NY, X4, Y4, 4, 3),
        !,
        NX == X4,
        NY == Y4,
        verifica_maximo(Tab, NX, NY, X5, Y5, 5, 3),
        !,
        NX == X5,
        NY == Y5,
        verifica_maximo(Tab, NX, NY ,X6, Y6, 6, 3),
        !,
        NX == X6,
        NY == Y6,
        verifica_maximo(Tab, NX, NY, X7, Y7, 7, 3),
        !,
        NX == X7,
        NY == Y7.

encontra_peca(Tab, P, X, Y):-
       (
          random(0, 5, X),
          random(0, 5, Y),
          busca_elemento(Tab, X, Y, P)
       ;
          encontra_peca(Tab, P, X, Y)
        ).

jogada_aleatoria(Tab, Xi, Yi, Xf, Yf, P):-
        (
           (
              P = 3
           ;
              encontra_peca(Tab, P, Xi, Yi)
           ),
           random(0, 8, M),
           verifica_maximo(Tab, Xi, Yi, Xf, Yf, M, P),
           valida_jogada(Tab, Xi, Yi, Xf, Yf, P)
        ;
           jogada_aleatoria(Tab, Xi, Yi, Xf, Yf, P) 
         ).

jogadas_possiveis(_,_,_,8,_,[]).
jogadas_possiveis(Tab, Xi, Yi, M,N, [[Xm,Ym]|T]):-
        verifica_maximo(Tab, Xi, Yi, Xm, Ym, M, N),
        valida_jogada(Tab, Xi, Yi, Xm, Ym, N),
        M1 is M +1,
        jogadas_possiveis(Tab, Xi, Yi, M1,N, T).
jogadas_possiveis(Tab, Xi, Yi, M,N, T):-
        M1 is M +1,
        jogadas_possiveis(Tab, Xi, Yi, M1,N, T).

melhor_jogada([],_,_,_):-
        !,false.
melhor_jogada([[X,Y]|_],X,X,Y).
melhor_jogada([_|T],X,Xf,Yf):-
        melhor_jogada(T,X,Xf,Yf).
        
elimina_jogadas([],_,[]).
elimina_jogadas([[X,_]|T],X,TR):-
        elimina_jogadas(T,X,TR).
elimina_jogadas([[X,Y]|T],X1,[[X,Y]|TR]):-
        elimina_jogadas(T,X1,TR).



jogada_inteligente_neutron(Tab, Xi, Yi, Xf, Yf, J):-
        jogadas_possiveis(Tab, Xi, Yi, 0, 3, R),
        (
           (
              J = 1,
              melhor_jogada(R, 0, Xf, Yf)
           ;
              melhor_jogada(R, 4, Xf, Yf)
           )
        ;
           (
              J = 1,
              elimina_jogadas(R, 4, RR)
           ;
              elimina_jogadas(R, 0, RR)
           ),
        random_select([Xf,Yf], RR, _)
        ;
        jogada_aleatoria(Tab, Xi, Yi, Xf, Yf, 3)
        ).

print_fim_de_jogo:-
        write('\n   __ _                 _          _                   '), nl,     
        write('  / _(_)_ __ ____    __| | ___    (_) ___   __ _  ___  '), nl,
        write(' | |_| |  _   _  |  / _  |/ _ |   | |  _  || _  || _   '), nl,
        write(' |  _| | | | | | | | (_| |  __/   | | (_) | (_| | (_) |'), nl,
        write(' |_| |_|_| |_| |_|  ___,_||___|  _/ | ___/  __, | ___/ '), nl,
        write('                                |__/       |___/       ').

server:-
        port(Port),
        socket_server_open(Port, Socket),
        socket_server_accept(Socket, _Client, Stream, [type(text)]),
        write('Accepted connection'), nl,
        serverLoop(Stream),
        socket_server_close(Socket).
% wait for commands



serverLoop(Stream) :-
        repeat,
        read(Stream, ClientMsg),
        write('Read: '), write(ClientMsg), nl,
        
        parse_input(ClientMsg, A),
        format(Stream, '~q.~n', [A]),
        write('Wrote: '), write(A), nl,
        flush_output(Stream),
        (ClientMsg == quit; ClientMsg == end_of_file), !.

parse_input(valida_jogada(Tab,Xi,Yi,Xf,Yf,N),1):-
        valida_jogada(Tab,Xi,Yi,Xf,Yf,N),!.
parse_input(valida_jogada(Tab,Xi,Yi,Xf,Yf,N),0).
        
parse_input(quit, ok-bye) :- !.