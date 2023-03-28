#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

typedef struct Player{
    int type;
} Player;

typedef struct Jogada{
	int tipo;
	int jogador;
	int comidos;
	int resultado;
	double tempo;
	time_t data;
	char board[7][5];
} Jogada;

typedef struct{
	int next_row, next_col;
	int old_row, old_col;
    int row, col;
    int new_row, new_col;
}Jogo;

void fim_de_jogo(char board[7][5], Player *jogador, Player *player, int option, int jogo, time_t start, FILE *file_ptr);
void recursos(char board[7][5], int option, Player *jogador, Player *player, Jogo *best_play, int  *recurso, time_t start, FILE *file_ptr);
int main();

Jogada historico[100];
int jogadas_realizadas = 0;

void atualizar_historico(int tipo, int resultado, double tempo) {
	historico[jogadas_realizadas].tipo = tipo;
	historico[jogadas_realizadas].resultado = resultado;
	historico[jogadas_realizadas].tempo = tempo;
	historico[jogadas_realizadas].data = time(NULL);
	jogadas_realizadas++;
}

void exibir_historico() {
    int i, jogos_pvp = 0, jogos_pvc = 0, vitorias = 0, derrotas = 0;
    double maior_tempo = 0, menor_tempo = 999999999;

    FILE *arquivo = fopen("historico.bin", "rb");
    if (arquivo == NULL) {
        printf("Não há histórico de jogadas.\n");
        return;
    }
	int contador = 0;
    while (fread(&historico[i], sizeof(Jogada), 1, arquivo) == 1) {
    	printf("Partida: %d\n", contador + 1);
        printf("Tipo: ");
        if (historico[i].tipo == 1) {
			printf("Player x Player\n");
			jogos_pvp++;
		} else {
			printf("Player x Computador\n");
			jogos_pvc++;
		}
        printf("Resultado: ");
		if (historico[i].resultado == 1) {
			printf("Vitoria\n");
			vitorias++;
		} else {
			printf("Derrota\n");
			derrotas++;
		}
        printf("Tempo: %.f segundos\n", historico[i].tempo);
        printf("Data: %s\n", ctime(&historico[i].data));
		if (historico[i].tempo > maior_tempo) maior_tempo = historico[i].tempo;
		if (historico[i].tempo < menor_tempo) menor_tempo = historico[i].tempo;
		contador++;
	}
	fclose(arquivo);
	
	printf("Estatisticas:\n");
	printf("Jogos P x P: %d\n", jogos_pvp);
	printf("Jogos P x C: %d\n", jogos_pvc);
	printf("Vitorias: %d\n", vitorias);
	printf("Derrotas: %d\n", derrotas);
	printf("Maior tempo: %.f segundos\n", maior_tempo);
	printf("Menor tempo: %.f segundos\n", menor_tempo);
	system("pause");
	system("cls");
}

void salvar_historico(int tipo, int resultado, double tempo) {
    Jogada jogada;
    jogada.tipo = tipo;
    jogada.resultado = resultado;
    jogada.tempo = tempo;
    historico[jogadas_realizadas] = jogada;
    jogada.data = time(NULL);

    FILE *arquivo = fopen("historico.bin", "ab");
    fwrite(&jogada, sizeof(Jogada), 1, arquivo);
    fclose(arquivo);
}

void print_board(char board[7][5], FILE *file_ptr){
	int i, j;
	
	for(i=0; i<7; i++){
    	for(j=0; j<5; j++){
			if(i == 5 && j == 0 || i == 5 && j == 4 || i == 6 && j == 1 || i == 6 && j == 3){
            	board[i][j] = ' ';
			}
		}
	}
	
	print_to_file_and_screen("\nAtual situacao do tabuleiro: \n\n", file_ptr);
	print_to_file_and_screen("  0   1   2   3   4\n", file_ptr);
	for (i = 0; i < 7; i++){	
		printf("%d ",i);		
	    for (j = 0; j < 5; j++){
	    	if(j<4 && i<5){
	    		printf("%c _ ", board[i][j]);
			}else if(j==4 || i>4){
	        	printf("%c   ", board[i][j]);
			}			
	    }
	    print_to_file_and_screen("\n", file_ptr);
	    if(i%2==0 && i<4){
	    	print_to_file_and_screen("  | \\ | / | \\ | / |", file_ptr);
		}else if(i!=0 && i<4){
			print_to_file_and_screen("  | / | \\ | / | \\ |", file_ptr);
		}else if(i==4){
			print_to_file_and_screen("     /    |    \\   ", file_ptr);
		}else if(i==5){
			print_to_file_and_screen("   /      |      \\   ", file_ptr);
		}    	
		print_to_file_and_screen("\n", file_ptr);    	
	} 
}

void exibir_salvo() {
    Jogada jogada;
    FILE *arquivo = fopen("saved_game.bin", "rb");
    if (arquivo == NULL) {
        printf("Nao ha jogo salvo.\n");
        return;
    }

    fread(&jogada, sizeof(Jogada), 1, arquivo);
    printf("Tipo: ");
    if (jogada.tipo == 1) {
		printf("Player x Player\n");
	} else {
		printf("Player x Computador\n");
	}
    printf("Jogador: %d\n", jogada.resultado);
    printf("Numero de cachorros comidos: %d\n", jogada.comidos);
    printf("Tempo: %.f segundos\n", jogada.tempo);
    printf("Estado do tabuleiro:\n\n");
	printf("  0   1   2   3   4\n");
    for (int i = 0; i < 7; i++) {
    	printf("%d ",i);
        for (int j = 0; j < 5; j++) {
            if(j<4 && i<5){
	    		printf("%c _ ", jogada.board[i][j]);
			}else if(j==4 || i>4){
	        	printf("%c   ", jogada.board[i][j]);
			}
        }
        printf("\n");
	    if(i%2==0 && i<4){
	    	printf("  | \\ | / | \\ | / |");
		}else if(i!=0 && i<4){
			printf("  | / | \\ | / | \\ |");
		}else if(i==4){
			printf("     /    |    \\   ");
		}else if(i==5){
			printf("   /      |      \\   ");
		}    	
		printf("\n"); 
    }
    fclose(arquivo);
    system("pause");
	system("cls");
}

void salvar_jogo(char board[7][5], int option, Player *jogador, Player *player, int comidos, time_t start) {
    Jogada jogada;
    jogada.tipo = option;
    jogada.jogador = jogador->type;
    jogada.comidos = comidos;
    jogada.tempo = difftime(time(NULL), start);
    memcpy(jogada.board, board, sizeof(jogada.board));

    FILE *arquivo = fopen("saved_game.bin", "wb");
    fwrite(&jogada, sizeof(Jogada), 1, arquivo);
    fclose(arquivo);
}

void print_to_file_and_screen(const char *format, FILE *file_ptr, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    vfprintf(file_ptr, format, args);
    va_end(args);
}

void init_game(int player_onca, int player_cachorro, Player player[], int option, FILE *file_ptr){
    
    print_to_file_and_screen("Jogador 1, escolha se quer ser a Onca (1) ou o Cachorro (2): ", file_ptr);
    scanf("%d", &player[0].type);

	if(option == 1){
		if (player[0].type == player_onca) {
			player[1].type = player_cachorro; 
			print_to_file_and_screen("\nJogador 1, voce jogara com a onca\n", file_ptr);
			print_to_file_and_screen("\nJogador 2, voce jogara com os cachorros\n", file_ptr);
		}else{
			player[1].type = player_onca;
			print_to_file_and_screen("\nJogador 1, voce jogara com os cachorros\n", file_ptr);
			print_to_file_and_screen("\nJogador 2, voce jogara com a onca\n", file_ptr);
		}
	}else{
		if (player[0].type == player_onca) {
			player[1].type = player_cachorro; 
			print_to_file_and_screen("\nJogador, voce jogara com a onca\n", file_ptr);
			print_to_file_and_screen("\nO computador jogara com os cachorros\n", file_ptr);
		}else{
			player[1].type = player_onca;
			print_to_file_and_screen("\nJogador, voce jogara com os cachorros\n", file_ptr);
			print_to_file_and_screen("\nO computador jogara com a onca\n", file_ptr);
		}
	}		
}

int update_board(char board[7][5], Player *jogador, int row, int col, int old_row, int old_col, int new_row, int new_col, int next_row, int next_col, Player player[]){  
	int player_onca = 1, player_cachorro = 2;
	int comidos = 0;
	if(row == next_row && col == next_col){
    	board[new_row][new_col] = '.';
    	comidos++;
	}
    board[old_row][old_col] = '.';
    
    if (jogador->type == player_onca) {
	    board[row][col] = 'O';
	} else {
	    board[row][col] = 'C';
	}
	return comidos;	
}

void give_hint(char board[7][5], Player *jogador, int old_row, int old_col, FILE *file_ptr){
	int i, j, ROWS = 5, COLS = 5, new_row, new_col, player_onca = 1, player_cachorro = 2; 
	int ADJACENT_ROWS [8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int ADJACENT_COLS [8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	int adj_row [5] = {-1, 0, 1, 0, 0};
	int adj_col [5] = {0, -1, 0, 1, 0};
	
	printf("\nJogadas possiveis da peca na posicao (%d, %d):\n", old_row, old_col);
	if(old_row == 1 && old_col == 1 || old_row == 1 && old_col == 3 || old_row == 2 && old_col == 2 || old_row == 3 && old_col == 1 || old_row == 3 && old_col == 3 || old_row == 0 && old_col == 2 || old_row == 2 && old_col == 0 || old_row == 2 && old_col == 4 || old_row == 4 && old_col == 2 || old_row == 4 && old_col == 0 || old_row == 4 && old_col == 4 || old_row == 0 && old_col == 0 || old_row == 0 && old_col == 4){
		for (i = 0; i < 8; i++){
	        new_row = old_row + ADJACENT_ROWS[i];
	        new_col = old_col + ADJACENT_COLS[i];
	        if (new_row >= 0 && new_row < ROWS && new_col >= 0 && new_col < COLS && board[new_row][new_col] == '.'){
			    printf("(%d, %d)\n", new_row, new_col);
			}	            
    	} 
	}else{
		for (i = 0; i < 5; i++){
	        new_row = old_row + adj_row[i];
	        new_col = old_col + adj_col[i];
	        if (new_row >= 0 && new_row < ROWS && new_col >= 0 && new_col < COLS && board[new_row][new_col] == '.'){
			    printf("(%d, %d)\n", new_row, new_col);
			}
	    }
	}
}

int pode_comer_cachorros(char board[7][5], Player *jogador, int old_row, int old_col, int new_row, int new_col, int next_row, int next_col) {
	int i, nr, nc, ROWS = 5, COLS = 5, player_onca = 1, player_cachorro = 2;
	int ADJACENT_ROWS [8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int ADJACENT_COLS [8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	int adj_row [5] = {-1, 0, 1, 0, 0};
	int adj_col [5] = {0, -1, 0, 1, 0};	
	int cont = 0;
	
	char jogador_char;

	if (jogador->type == player_onca) {
	  jogador_char = 'O';
	} else {
	  jogador_char = 'C';
	}
	if(old_row == 1 && old_col == 1 || old_row == 1 && old_col == 3 || old_row == 2 && old_col == 2 || old_row == 3 && old_col == 1 || old_row == 3 && old_col == 3 || old_row == 0 && old_col == 2 || old_row == 2 && old_col == 0 || old_row == 2 && old_col == 4 || old_row == 4 && old_col == 2 || old_row == 4 && old_col == 0 || old_row == 4 && old_col == 4 || old_row == 0 && old_col == 0 || old_row == 0 && old_col == 4){
		for (i = 0; i < 8; i++){
			nr = old_row + ADJACENT_ROWS[i];
			nc = old_col + ADJACENT_COLS[i];
			if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != jogador_char && board[nr][nc] != '.'){
				next_row = nr + ADJACENT_ROWS[i];
				next_col = nc + ADJACENT_COLS[i];
				if (next_row >= 0 && next_row < ROWS && next_col >= 0 && next_col < COLS && board[next_row][next_col] == '.'){
					new_row = nr;
					new_col = nc;
					cont++;
					nr = next_row + ADJACENT_ROWS[i];
					nc = next_col + ADJACENT_COLS[i];
					if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != jogador_char && board[nr][nc] != '.'){
						next_row = nr + ADJACENT_ROWS[i];
						next_col = nc + ADJACENT_COLS[i];
						if (next_row >= 0 && next_row < ROWS && next_col >= 0 && next_col < COLS && board[next_row][next_col] == '.'){
							new_row = nr;
							new_col = nc;
							cont++;
						}
					}
			    }
			}
		} 	
		return cont;
	}else{
		for (i = 0; i < 5; i++) {
	        nr = old_row + adj_row[i];
	        nc = old_col + adj_col[i];
	        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != jogador_char && board[nr][nc] != '.') {
	            next_row = nr + adj_row[i];
	            next_col = nc + adj_col[i];
	            if (next_row >= 0 && next_row < ROWS && next_col >= 0 && next_col < COLS && board[next_row][next_col] == '.') {
	                new_row = nr;
	                new_col = nc;
	                cont++;
	                break;
	            }
	        }
	    }
	    return cont;
	}
}

int peca_adjacente(char board[7][5], Player *jogador, int old_row, int old_col, int *new_row, int *new_col, int *next_row, int *next_col) {
	int i, nr, nc, ROWS = 5, COLS = 5, player_onca = 1, player_cachorro = 2;
	int ADJACENT_ROWS [8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int ADJACENT_COLS [8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	int adj_row [5] = {-1, 0, 1, 0, 0};
	int adj_col [5] = {0, -1, 0, 1, 0};
	int cont = 0;
	char jogador_char;
	
	if (jogador->type == player_onca) {
		jogador_char = 'O';
	} else {
		jogador_char = 'C';
	}
	if(old_row == 1 && old_col == 1 || old_row == 1 && old_col == 3 || old_row == 2 && old_col == 2 || old_row == 3 && old_col == 1 || old_row == 3 && old_col == 3 || old_row == 0 && old_col == 2 || old_row == 2 && old_col == 0 || old_row == 2 && old_col == 4 || old_row == 4 && old_col == 2 || old_row == 4 && old_col == 0 || old_row == 4 && old_col == 4 || old_row == 0 && old_col == 0 || old_row == 0 && old_col == 4){
		for (i = 0; i < 8; i++) {
			nr = old_row + ADJACENT_ROWS[i];
			nc = old_col + ADJACENT_COLS[i];
			if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != jogador_char && board[nr][nc] != '.') {
				*next_row = nr + ADJACENT_ROWS[i];
				*next_col = nc + ADJACENT_COLS[i];
				if (*next_row >= 0 && *next_row < ROWS && *next_col >= 0 && *next_col < COLS && board[*next_row][*next_col] == '.') {
					*new_row = nr;
					*new_col = nc;
					cont++;
					break;
				}
			}
		}
		return cont;
	}else{
		for (i = 0; i < 5; i++) {
	        nr = old_row + adj_row[i];
	        nc = old_col + adj_col[i];
	        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS && board[nr][nc] != jogador_char && board[nr][nc] != '.') {
	            *next_row = nr + adj_row[i];
	            *next_col = nc + adj_col[i];
	            if (*next_row >= 0 && *next_row < ROWS && *next_col >= 0 && *next_col < COLS && board[*next_row][*next_col] == '.') {
	                *new_row = nr;
	                *new_col = nc;
	                cont++;
	                break;
	            }
	        }
	    }
	    return cont;
	}
}

int is_valid_move(char board[7][5], Player *jogador, int old_row, int old_col, int row, int col) {
	if (old_row == row && old_col == col) {
        return 0;
    }
    if (row < 0 || row >= 5 || col < 0 || col >= 5) {
        return 0;
    }
    if(old_row == 1 && old_col == 1 || old_row == 1 && old_col == 3 || old_row == 2 && old_col == 2 || old_row == 3 && old_col == 1 || old_row == 3 && old_col == 3 || old_row == 0 && old_col == 2 || old_row == 2 && old_col == 0 || old_row == 2 && old_col == 4 || old_row == 4 && old_col == 2 || old_row == 4 && old_col == 0 || old_row == 4 && old_col == 4 || old_row == 0 && old_col == 0 || old_row == 0 && old_col == 4){
    	if (abs(row - old_row) <= 1 && abs(col - old_col) <= 1 && board[row][col] == '.'){
		    return 1; 
		}else{
			return 0;
		}
	}else{
		if ((row == old_row && (col == old_col + 1 || col == old_col - 1)) || (col == old_col && (row == old_row + 1 || row == old_row - 1))) {
			if( board[row][col] == '.'){
				return 1; 
			}else{
				return 0;
			}		    
		} else {
		    return 0;
		}
	}
}

int ultima_linha = -1;
int ultima_coluna = -1;

int valida_computador(char board[7][5], Player *jogador, int old_row, int old_col, int *row, int *col) {
	int delta_row, delta_col;
	for (delta_row = -1; delta_row <= 1; delta_row++) {
		for (delta_col = -1; delta_col <= 1; delta_col++) {
			*row = old_row + delta_row;
			*col = old_col + delta_col;
			if (old_row == *row && old_col == *col) {
				continue; 
			}
			if (*row < 0 || *row >= 5 || *col < 0 || *col >= 5) {
				continue; 
			}
			if(old_row == 1 && old_col == 1 || old_row == 1 && old_col == 3 || old_row == 2 && old_col == 2 || old_row == 3 && old_col == 1 || old_row == 3 && old_col == 3 || old_row == 0 && old_col == 2 || old_row == 2 && old_col == 0 || old_row == 2 && old_col == 4 || old_row == 4 && old_col == 2 || old_row == 4 && old_col == 0 || old_row == 4 && old_col == 4 || old_row == 0 && old_col == 0 || old_row == 0 && old_col == 4){
				if (abs(*row - old_row) <= 1 && abs(*col - old_col) <= 1 && board[*row][*col] == '.' && (*row != ultima_linha || *col != ultima_coluna)){
					ultima_linha = *row;
					ultima_coluna = *col;
					return 1;
				}else{
					continue;
				}
			}else{
				if ((*row == old_row && (*col == old_col + 1 || *col == old_col - 1)) || (*col == old_col && (*row == old_row + 1 || *row == old_row - 1))) {
					if( board[*row][*col] == '.' && (*row != ultima_linha || *col != ultima_coluna)){
						ultima_linha = *row;
						ultima_coluna = *col;
						return 1;
					}else{
						continue;
					}
				} else {
					continue;
				}
			}
		}
	}
	return 0;
}

void posicao_onca(char board[7][5], int *old_row, int *old_col){
	int i, j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 5; j++) {
			if (board[i][j] == 'O') {
				*old_row = i;
				*old_col = j;
			}
		}
	}
}

int end_game(char board[7][5], Player *jogador, Player *player, int option, int jogo, double tempo, int vencedor) {
    if (vencedor == 1) {
        atualizar_historico(option, 1, tempo);
        return 1;
    } else if (vencedor == 2) {
        atualizar_historico(option, 2, tempo);
        return 2;
    } else {
        atualizar_historico(option, 0, tempo);
        return 0;
    }
}

int comidos = 0;

void jogada_onca(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr){
	int old_row, old_col, cont = 0, voltas = 0, continua, row, col, next_row, next_col, new_row, new_col, posicao_valida, i, j, nr, nc;
	int ADJACENT_ROWS [8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int ADJACENT_COLS [8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	char jogador_char;
	if (jogador->type == 1) {
		jogador_char = 'O';
	} else {
		jogador_char = 'C';
	}
	posicao_onca(board, &old_row, &old_col);
	cont = pode_comer_cachorros(board, jogador, old_row, old_col, new_row, new_col, next_row, next_col);
	if(cont == 0){
		print_board(board, file_ptr);
		give_hint(board, jogador, old_row, old_col, file_ptr);
	}
	if(cont>0){
		do{
			if(voltas >= 0){
				print_board(board, file_ptr);
			}
			printf("\nNa posicao que voce esta, voce pode comer %d cachorro(s)!\n", cont);
			for (i = 0; i < 8; i++){
				nr = old_row + ADJACENT_ROWS[i];
				nc = old_col + ADJACENT_COLS[i];
				if (nr >= 0 && nr < 5 && nc >= 0 && nc < 5 && board[nr][nc] != jogador_char && board[nr][nc] != '.'){
				    next_row = nr + ADJACENT_ROWS[i];
				    next_col = nc + ADJACENT_COLS[i];
				    if (next_row >= 0 && next_row < 5 && next_col >= 0 && next_col < 5 && board[next_row][next_col] == '.'){	
						printf("\nO cachorro na posicao (%d, %d) pode ser comido.\nPara come-lo, voce precisa ir para a posicao (%d, %d)\n", nr, nc, next_row, next_col);
						new_row = nr;
				       	new_col = nc;	
				       	break;
				    }
				}
			}
			print_to_file_and_screen("\nDeseja realizar esta jogada?\n1 - Sim\n2 - Nao\n", file_ptr);			
			scanf("%d", &continua);
			if (continua == 1) {
				print_to_file_and_screen("\nOnca, insira sua jogada: ", file_ptr);
				scanf("%d %d", &row, &col);	
			    while(row != next_row || col != next_col){
			    	print_to_file_and_screen("Posicao invalida, informe novamente: ", file_ptr);
			        scanf("%d %d", &row, &col);
				}
				update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
		    	system("cls");	
		    	if(row == next_row && col == next_col){
				   	comidos++;
				}
				if(comidos >= 5){
					print_board(board, file_ptr);
					printf("Parabens jogador %d, a onca ganhou e comeu %d cachorros!\n", jogador->type, comidos);
					print_to_file_and_screen("Fim de jogo.\n", file_ptr);
					fclose(file_ptr);
					time_t end = time(NULL);
					double tempo = difftime(end, start);
					int vencedor = 1;
					int resultado = end_game(board, jogador, player, option, jogo, tempo, vencedor);
					salvar_historico(option, resultado, tempo);
					system("pause");
					system("cls");
					fim_de_jogo(board, jogador, player, option, jogo, start, file_ptr);
					exit(0);
				}				
		    	posicao_onca(board, &old_row, &old_col);
				cont = pode_comer_cachorros(board, jogador, old_row, old_col, new_row, new_col, next_row, next_col);
			}else{
				print_to_file_and_screen("\nOnca, insira sua jogada: ", file_ptr);
				scanf("%d %d", &row, &col);
				posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
				while (posicao_valida == 0) {
				    print_to_file_and_screen("Posicao invalida, informe novamente: ", file_ptr);
				    scanf("%d %d", &row, &col);
				    posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
				}
			    update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
				system("cls");				
				voltas++;
			}
		}while(cont>0 && continua == 1);
	}else{
		print_to_file_and_screen("\nOnca, insira sua jogada: ", file_ptr);
		scanf("%d %d", &row, &col);
		posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
		while (posicao_valida == 0) {
		    print_to_file_and_screen("Posicao invalida, informe novamente: ", file_ptr);
		    scanf("%d %d", &row, &col);
		    posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
		}
	   	update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
	    system("cls");
	}		
}

void vez_onca(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr){
	int resposta, new_row, new_col, next_row, next_col, cont = 0, old_row, old_col, row, col, escolha, escolha2, recurso, dog_eat = 0;	
	Jogo *best_play = (Jogo*) malloc(sizeof(Jogo));
	
	print_to_file_and_screen("\nDeseja utilizar o recurso de dica, salvar ou pausar?\n1- Sim\n2- Nao\n", file_ptr);
	scanf("%d", &resposta);
	if(resposta == 1){
		system("pause");
		system("cls");
		recursos(board, option, jogador, player, best_play, &recurso, start, file_ptr);
		if(recurso == 1){
			print_board(board, file_ptr);
            printf("A melhor jogada e: (%d, %d)\n", best_play->row, best_play->col);
            print_to_file_and_screen("Deseja fazer essa jogada?\n1- Sim\n2- Nao\n", file_ptr);
            scanf("%d", &escolha);
            if(escolha == 1){
            	if(best_play->row == best_play->next_row && best_play->col == best_play->next_col && escolha == 1){
	            	comidos++;
				}
				dog_eat = update_board(board, jogador, best_play->row, best_play->col, best_play->old_row, best_play->old_col, best_play->new_row, best_play->new_col, best_play->next_row, best_play->next_col, player);
                system("pause");
                system("cls");
                posicao_onca(board, &old_row, &old_col);
				cont = peca_adjacente(board, jogador, old_row, old_col, &new_row, &new_col, &next_row, &next_col);
                while (cont > 0 && dog_eat>0){
					posicao_onca(board, &old_row, &old_col);
					cont = peca_adjacente(board, jogador, old_row, old_col, &new_row, &new_col, &next_row, &next_col);
					if (cont > 0) {
						print_board(board, file_ptr);
						printf("A onca ainda pode comer cachorro(s). Para isso, precisa ir para a posicao (%d, %d)\n", next_row, next_col);
						print_to_file_and_screen("Deseja fazer essa jogada?\n1- Sim\n2- Nao\n", file_ptr);
						scanf("%d", &escolha2);
						system("pause");
						system("cls");
						if (escolha2 == 1) {
							comidos++;
							update_board(board, jogador, next_row, next_col, old_row, old_col, new_row, new_col, next_row, next_col, player);
						} else {
							cont = 0;
						}
					}
				}
			} else {
				system("pause");
				system("cls");
				jogada_onca(board, jogador, player, option, jogo, start, file_ptr);
			}
		}else if(recurso == 2){
				jogada_onca(board, jogador, player, option, jogo, start, file_ptr);
		}else{
			print_to_file_and_screen("Obrigada por jogar, ate a proxima!", file_ptr);
			fclose(file_ptr);
			exit(0);
		}
	}else{
		system("pause");
		system("cls");
		resposta = 2;
		jogada_onca(board, jogador, player, option, jogo, start, file_ptr);	
	}
	free(best_play);
}

int vence_cachorro(char board[7][5]){
	int ADJACENT_ROWS [8] = {-1, -1, -1, 0, 0, 1, 1, 1};
	int ADJACENT_COLS [8] = {-1, 0, 1, -1, 1, -1, 0, 1};
	int nr, nc, old_row, old_col, tem_jogada_possivel = 0, i;
	
	posicao_onca(board, &old_row, &old_col);
	tem_jogada_possivel = 0;
	for (i = 0; i < 8; i++){
	   nr = old_row + ADJACENT_ROWS[i];
	   nc = old_col + ADJACENT_COLS[i];
	   if (nr >= 0 && nr < 5 && nc >= 0 && nc < 5 && board[nr][nc] == '.'){
	      tem_jogada_possivel = 1;
	      break;
	   }
	}
	return tem_jogada_possivel;
}

int is_surrounded(char board[7][5], int row, int col) {
    int i, j, surrounded = 0;

    for (i = row - 1; i <= row + 1; i++) {
        for (j = col - 1; j <= col + 1; j++) {
            if (i >= 0 && i < 7 && j >= 0 && j < 5 && (i != row || j != col) && board[i][j] == '.') {
                surrounded = 1;
                break;
            }
        }
    }
    return surrounded;
}

void jogada_dog(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr){
	int old_row, old_col, row, col, posicao_valida, new_row, new_col, next_row, next_col, tem_jogada_possivel, surrounded;
	
	print_to_file_and_screen("Cachorros, insira a posicao da peca que deseja movimentar: ", file_ptr);
	scanf("%d %d", &old_row, &old_col);
	while(board[old_row][old_col] != 'C'){
		print_to_file_and_screen("Nao ha nenhum cachorro na posicao, informe novamente: ", file_ptr);
		scanf("%d %d", &old_row, &old_col);
	}
	surrounded = is_surrounded(board, old_row, old_col);
	while(surrounded == 0) {
	   print_to_file_and_screen("peca encurralada, escolha outra posicao \n", file_ptr);
	   scanf("%d %d", &old_row, &old_col);
	   surrounded = is_surrounded(board, old_row, old_col);
	}
    give_hint(board, jogador, old_row, old_col, file_ptr);
    print_to_file_and_screen("\nAgora insira a posicao que deseja ir: ", file_ptr);
    scanf("%d %d", &row, &col);
    posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
	while (posicao_valida == 0) {
	    print_to_file_and_screen("Posicao invalida, informe novamente: ", file_ptr);
	    scanf("%d %d", &row, &col);
	    posicao_valida = is_valid_move(board, jogador, old_row, old_col, row, col);
	}
    update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
	system("cls");
	    
	tem_jogada_possivel = vence_cachorro(board);
	if (tem_jogada_possivel == 0){		
	    print_board(board, file_ptr);
	    printf("Parabens jogador %d, os cachorros ganharam e encurralaram a onca!\n", jogador->type);
		print_to_file_and_screen("Fim de jogo.\n", file_ptr);
		fclose(file_ptr);
		time_t end = time(NULL);
		double tempo = difftime(end, start);
		int vencedor = 2;
		int resultado = end_game(board, jogador, player, option, jogo, tempo, resultado);
		salvar_historico(option, resultado, tempo);
		system("pause");
		system("cls");
		fim_de_jogo(board, jogador, player, option, jogo, start, file_ptr);
		exit(0);
	}
}

void vez_cachorros(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr){
	int row, col, new_row, new_col, next_row, next_col, player_onca = 1, player_cachorro = 2, posicao_valida, resposta, cont = 0, recurso; 
	char jogador_char;
	Jogo *best_play = (Jogo*) malloc(sizeof(Jogo));
	
	print_to_file_and_screen("\nDeseja utilizar o recurso de dica, salvar ou pausar?\n1- Sim\n2- Nao\n", file_ptr);
	scanf("%d", &resposta);
	if(resposta == 1){
		system("pause");
		system("cls");
		recursos(board, option, jogador, player, best_play, &recurso, start, file_ptr);
		if(recurso == 1){
			if(best_play->row != -1 && best_play->col != -1){
				print_board(board, file_ptr);
	            printf("A melhor jogada e: posicao (%d, %d) para (%d, %d)\n", best_play->old_row, best_play->old_col, best_play->row, best_play->col);
	            print_to_file_and_screen("Deseja fazer essa jogada?\n1- Sim\n2- Nao\n", file_ptr);
	            int escolha;
	            scanf("%d", &escolha);
	            if(escolha == 1){
	                update_board(board, jogador, best_play->row, best_play->col, best_play->old_row, best_play->old_col, new_row, new_col, best_play->next_row, best_play->next_col, player);
	                system("pause");
	                system("cls");
	            }else{
	            	jogada_dog(board, jogador, player, option, jogo, start, file_ptr);				
				}
	        }	
		}else if(recurso == 2){
			jogada_dog(board, jogador, player, option, jogo, start, file_ptr);
		}else{
			print_to_file_and_screen("Obrigada por jogar, ate a proxima!", file_ptr);
			fclose(file_ptr);
			exit(0);
		}
	}else{
		jogada_dog(board, jogador, player, option, jogo, start, file_ptr);
	}
	free(best_play);
}

void jogada_computador_onca(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr) {	
    int old_row, old_col, i, j, new_row, new_col, row, col, next_row, next_col, jogada_valida;    
	posicao_onca(board, &old_row, &old_col);
	int cont = peca_adjacente(board, jogador, old_row, old_col, &new_row, &new_col, &next_row, &next_col);	 
	jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
	if (cont > 0) {	
		while (peca_adjacente(board, jogador, old_row, old_col, &new_row, &new_col, &next_row, &next_col) > 0) {
		    jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
		    if (jogada_valida) {
		        row = next_row;
		        col = next_col;
		        comidos++;
		        update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
		        system("pause");
		        system("cls");
		        if (comidos >= 5) {
		            print_board(board, file_ptr);
		            printf("O computador (onca) ganhou e comeu %d cachorros!\n", comidos);
		            print_to_file_and_screen("Fim de jogo.\n", file_ptr);
		            fclose(file_ptr);
		            time_t end = time(NULL);
		            double tempo = difftime(end, start);
		            int vencedor = 1;
		            int resultado = end_game(board, jogador, player, option, jogo, tempo, resultado);
		            salvar_historico(option, resultado, tempo);
		            system("pause");
		            system("cls");
		            fim_de_jogo(board, jogador, player, option, jogo, start, file_ptr);
		            exit(0);
		        }
		    }
		    print_board(board, file_ptr);
		    posicao_onca(board, &old_row, &old_col);
		}
		update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
		system("pause");
		system("cls");			
	} else {		
		jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);		
		while(jogada_valida == 0){
			jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
		}
		update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
		system("pause");
		system("cls");
	}	
}

int last_dog_row = 0, last_dog_col = 0;

void jogada_cachorro(char board[7][5], int *row, int *col, int *old_row, int *old_col, int *jogada_valida, Player *jogador) {
	int i, j;
	int found = 0;
	*jogada_valida = 0;
	for (i = last_dog_row; i < 5; i++) {
		for (j = last_dog_col; j < 5; j++) {
			if (board[i][j] == 'C') {
				int new_row = i, new_col = j;
				if (i < 5 - 1 && board[i + 1][j] == '.') {
					new_row = i + 1;
				} else if (i > 0 && board[i - 1][j] == '.') {
					new_row = i - 1;
				} else if (j < 5 - 1 && board[i][j + 1] == '.') {
					new_col = j + 1;
				} else if (j > 0 && board[i][j - 1] == '.') {
					new_col = j - 1;
				}
				*jogada_valida = valida_computador(board, jogador, i, j, &new_row, &new_col);
				if (*jogada_valida) {
					*old_row = i;
					*old_col = j;
					*row = new_row;
					*col = new_col;
					last_dog_row = i;
					last_dog_col = j;
					found = 1;
					break;
				}
			}
		}
		if (found) break;
	}
	if (!found) {
		last_dog_row = 0;
		last_dog_col = 0;
		jogada_cachorro(board, row, col, old_row, old_col, jogada_valida, jogador);
	}
}

void jogada_computador_cachorro(char board[7][5], Player *jogador, Player player[], int option, int jogo, time_t start, FILE *file_ptr) {
	int old_row = -1, old_col = -1, row, col, jogada_valida, new_row, new_col, next_row, next_col, tem_jogada_possivel;
	jogada_cachorro(board, &row, &col, &old_row, &old_col, &jogada_valida, jogador);
	while (!jogada_valida) {
		jogada_cachorro(board, &row, &col, &old_row, &old_col, &jogada_valida, jogador);
	}
	update_board(board, jogador, row, col, old_row, old_col, new_row, new_col, next_row, next_col, player);
	system("pause");
	system("cls");
	
	tem_jogada_possivel = vence_cachorro(board);
	if (tem_jogada_possivel == 0){		
	    print_board(board, file_ptr);
	    print_to_file_and_screen("O computador ganhou e os cachorros encurralaram a onca!\n", file_ptr);
		print_to_file_and_screen("Fim de jogo.\n", file_ptr);
		fclose(file_ptr);
		time_t end = time(NULL);
		double tempo = difftime(end, start);
		int vencedor = 2;
		int resultado = end_game(board, jogador, player, option, jogo, tempo, resultado);
		salvar_historico(option, resultado, tempo);
		system("pause");
		system("cls");
		fim_de_jogo(board, jogador, player, option, jogo, start, file_ptr);
		exit(0);
	}
}

void play_game(char board[7][5], Player *jogador, int vez, Player player[], int option, int jogo, time_t start, FILE *file_ptr){
	int player_onca = 1, player_cachorro = 2;
	Jogo *best_play = (Jogo*) malloc(sizeof(Jogo));
	
	print_board(board, file_ptr);
	if(option == 1){
		if (jogador == &player[0]) {
		    print_to_file_and_screen("\nJogador 1, sua vez!\n", file_ptr);
		} else {
		    print_to_file_and_screen("\nJogador 2, sua vez!\n", file_ptr);
		}
		
		if (jogador->type == player_onca) {
		    vez_onca(board, jogador, player, option, jogo, start, file_ptr);
		} else {
		    vez_cachorros(board, jogador, player, option, jogo, start, file_ptr);
		}
	}else{
		if (jogador == &player[0]) {
		    print_to_file_and_screen("\nJogador, sua vez!\n", file_ptr);
		} else {
		    print_to_file_and_screen("\nVez do computador\n", file_ptr);
		}
		
		if (jogador->type == player_onca) {
			if(jogador->type == player[0].type){
				vez_onca(board, jogador, player, option, jogo, start, file_ptr);
			}else{
				jogada_computador_onca(board, jogador, player, option, jogo, start, file_ptr);
			}		    
		} else if(jogador->type == player_cachorro){
			if(jogador->type == player[0].type){
				vez_cachorros(board, jogador, player, option, jogo, start, file_ptr);
			}else{
				jogada_computador_cachorro(board, jogador, player, option, jogo, start, file_ptr);
			}		    
		}
	}
	free(best_play);
}

void chama_jogo(char board[7][5], Player *jogador, Player *player, int option, int jogo, time_t start, FILE *file_ptr){
	int player_onca = 1, player_cachorro = 2;
	if (player[1].type == player_onca) {
		play_game(board, &player[1], 1, player, option, jogo, start, file_ptr);
		play_game(board, &player[0], 2, player, option, jogo, start, file_ptr);
	} else {
	    play_game(board, &player[0], 1, player, option, jogo, start, file_ptr);
	    play_game(board, &player[1], 2, player, option, jogo, start, file_ptr);
	}
}

void init_board(char board[7][5]){
	int i, j;
	for (i = 0; i < 7; i++){
    	for (j = 0; j < 5; j++){
    		board[i][j] = '.';
    		if(i == 5 && j == 0 || i == 5 && j == 4 || i == 6 && j == 1 || i == 6 && j == 3){
            	board[i][j] = ' ';
			}
        }    	
	}

    for (i = 0; i < 3; i++)
        for (j = 0; j < 5; j++)
            board[i][j] = 'C';
            
    board[2][2] = 'O';
}

void fim_de_jogo(char board[7][5], Player *jogador, Player *player, int option, int jogo, time_t start, FILE *file_ptr){
	int opcao;
	print_board(board, file_ptr);
	print_to_file_and_screen("Escolha uma opcao abaixo: ", file_ptr);
	print_to_file_and_screen("\n1 - Jogar novamente\n2 - Voltar ao menu\n", file_ptr);
	scanf("%d", &opcao);
	switch(opcao){
		case 1: 
			system("pause");
			system("cls");
			init_board(board);
			chama_jogo(board, jogador, player, option, jogo, start, file_ptr);
			break;
		case 2:
			system("pause");
			system("cls");
			main();
			break;
	}
}

void dica_player(char board[7][5], Player *jogador, Player player[], Jogo *best_play) {
    int new_row, new_col, next_row, next_col, old_row, old_col, cont = 0, jogada_valida, row, col;
    
    if(jogador->type == 1){
    	posicao_onca(board, &old_row, &old_col);
    	best_play->old_row = old_row;
		best_play->old_col = old_col;
    	cont = peca_adjacente(board, jogador, old_row, old_col, &new_row, &new_col, &next_row, &next_col);
		jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);	
		if(cont>0 && jogada_valida == 1){
			best_play->row = next_row;
			best_play->col = next_col;
			best_play->next_row = next_row;
			best_play->next_col = next_col;
			best_play->new_row = new_row;
			best_play->new_col = new_col;
		} else{
			jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
			best_play->row = row;
			best_play->col = col;
		}		
	}else{
		jogada_cachorro(board, &row, &col, &old_row, &old_col, &jogada_valida, jogador);
		best_play->row = row;
		best_play->col = col;
		best_play->old_row = old_row;
		best_play->old_col = old_col;
	}
}

void closest_dog(char board[7][5], int *old_row, int *old_col, int *row, int *col, Player *jogador) {
    int i, j, curr_distance, onca_row, onca_col;
    int min_distance;
    min_distance = INT_MAX; 
    posicao_onca(board, &onca_row, &onca_col);

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (board[i][j] == 'C') { 
                curr_distance = abs(i - onca_row) + abs(j - onca_col); 
                if (curr_distance < min_distance) { 
                    min_distance = curr_distance; 
                    *old_row = i; 
                    *old_col = j;
                }
            }
        }
    }
}

void dica_pc(char board[7][5], Player *jogador, Player player[], Jogo *best_play) {
    int new_row, new_col, next_row, next_col, old_row, old_col, cont = 0, jogada_valida, row, col;
    if(jogador->type == 1){
    	posicao_onca(board, &old_row, &old_col);
    	best_play->old_row = old_row;
		best_play->old_col = old_col;
		jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
		while(jogada_valida == 0){
			jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
		}
		best_play->row = row;
		best_play->col = col;
	}else{
		closest_dog(board, &old_row, &old_col, &row, &col, jogador);
	    //jogada_cachorro(board, &row, &col, &old_row, &old_col, &jogada_valida, jogador); 
	    jogada_valida = valida_computador(board, jogador, old_row, old_col, &row, &col);
	    /*if(jogada_valida == 0){
	    	jogada_cachorro(board, &row, &col, &old_row, &old_col, &jogada_valida, jogador);
		}*/
		best_play->row = row;
		best_play->col = col;
		best_play->old_row = old_row;
		best_play->old_col = old_col;
	}
}

void recursos(char board[7][5], int option, Player *jogador, Player *player, Jogo *best_play, int *recurso, time_t start, FILE *file_ptr){
    int opcao;
    print_board(board, file_ptr);
    print_to_file_and_screen("Escolha uma opcao:\n1 - Dica\n2 - Pausar\n3 - Salvar\n", file_ptr);
    scanf("%d", &opcao);
    *recurso = opcao;
    switch(opcao){
        case 1: 
        	if(option == 1){
        		dica_player(board, jogador, player, best_play);
	            system("pause");
	            system("cls");
	            break;	
			}else{
				dica_pc(board, jogador, player, best_play);
				system("pause");
	            system("cls");
	            break;
			}
            	
        case 2:
            print_to_file_and_screen("Jogo pausado\n", file_ptr);
			time_t pause_time = difftime(time(NULL), start);
			system("pause");
			start = time(NULL);
			start += pause_time;
			system("cls");
			break;
		case 3: 
			print_to_file_and_screen("\nJogo salvo\n", file_ptr);
			print_to_file_and_screen("Se desejar continuar essa partida posteriormente, basta selecionar o 'historico' no menu\n", file_ptr);
			salvar_jogo(board, option, jogador, player, comidos, start);
			system("pause");
			system("cls");
			break;
    }
}

int main(){			
	int player_onca = 1, player_cachorro = 2, cont = 0, vez, opcao, jogo, option;
	Player player[2], *jogador;
	char board[7][5];
	time_t end, start;
	FILE *file_ptr;
	file_ptr = fopen("output.txt", "a");
	print_to_file_and_screen("Bem vindo ao Jogo da Onca!\n", file_ptr);

	start = time(NULL);
	init_board(board);
	do{		
		print_to_file_and_screen("\nEscolha uma das opcoes abaixo:\n", file_ptr);
		print_to_file_and_screen("\n 1 Jogar\n 2 Ajuda\n 3 Historico\n 4 Sair\n\n", file_ptr);
		scanf("%d", &opcao);
		system("cls");
		switch(opcao){
			case 1:
				print_to_file_and_screen("Escolha uma opcao de jogo:\n", file_ptr);
				print_to_file_and_screen("\n1 Player x Player\n2 Player x Computador\n\n", file_ptr);
				scanf("%d", &jogo);
				system("cls");
				switch(jogo){
					case 1:
						init_game(player_onca, player_cachorro, player, 1, file_ptr);
						system("pause");
						system("cls");	
						do{
							option = 1;
							chama_jogo(board, jogador, player, option, jogo, start, file_ptr);							
						}while(cont<2);	
						system("pause");
						system("cls");						
						break;
					case 2:
						init_game(player_onca, player_cachorro, player, 2, file_ptr);
						system("pause");
						system("cls");	
						do{
							option = 2;
							chama_jogo(board, jogador, player, option, jogo, start, file_ptr);
						}while(cont<2);
						system("pause");
						system("cls");	
						break;
				}					
				break;
			case 2:
				print_to_file_and_screen("\n_____________________________O QUE E O JOGO?_______________________________", file_ptr);
                print_to_file_and_screen("\n\t     O jogo da Onca e constituido por um tabuleiro chamado suetonio,\n\tque possui 31 posicoes, estando 6 delas na 'toca da onca'.\n\tEla possui 15 pecas, sendo 14 delas cachorros e uma, a onca.\n", file_ptr);
                print_to_file_and_screen("\n_____________________________  O OBJETIVO  _______________________________", file_ptr);
                print_to_file_and_screen("\n\n\t      Vencem os cachorros se eles encurralarem a onca,\n\tou vence a onca se ela comer mais de 5 cachorros!\n\n", file_ptr);                     
                print_to_file_and_screen("\n______________________________REGRAS DO JOGO_________________________________", file_ptr);
                print_to_file_and_screen("\n\n\t1-So e permitido se movimentar se houver uma casa vaga.\n\t3-So anda uma casa por vez.\n\t4-E possivel andar onde existe barras.\n\t5-Nao e permitido jogar com a peca do adversario.\n", file_ptr);
                print_to_file_and_screen("____________________________________________________________________________\n\n", file_ptr);               
                system("pause");
				system("cls");
				break;
        	case 3:
        		print_to_file_and_screen("Selecione uma opcao:\n\n1 - Historico\n2 - Jogo salvo\n", file_ptr);
        		int choice;
				scanf("%d", &choice);
				switch(choice){
					case 1:
						system("pause");
						system("cls");
						exibir_historico();
						break;
					case 2:
						system("pause");
						system("cls");
						exibir_salvo();
						break;	
				}
			    break;
			case 4:
				print_to_file_and_screen("\n\n\n\n\t\t\t  FIM DO JOGO\n\n\n\n\n", file_ptr);
				fclose(file_ptr);
                break;        		
		}
	}while(opcao!=4);
	return 0;
}