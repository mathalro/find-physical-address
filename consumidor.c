/**
	Prática de Sistemas Operacionais
		Imprimir endereco lógico e físico de um ponteiro para memória compartilhada
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>


#define PAGEMAP_LENGTH 8
#define PAGE_SHIFT 12

/*
	Funcao responsavel por encontrar o endereco do frame em que se encontra o ponteiro passado como parametro
*/
unsigned long getNumeroPageFrame(void *addr) {
	// le o arquivo que armazena o endereco fisico dos frames de cada página mapeada
    FILE *pagemap = fopen("/proc/self/pagemap", "rb");
    // Pega apenas o offset do enderecamento
    // Para isso, divide-se o endereco pelo tamanho de pagina do sistema (4096). 
    // Multiplica-se pelo comprimento do mapeamento de pagina para parar o ponteiro no início do mapeamento deste quadro 
    unsigned long offset = (unsigned long) addr / getpagesize() * PAGEMAP_LENGTH;
   	// anda com o ponteiro para a posição do quadro no arquivo pagemap
	fseek(pagemap, (unsigned long)offset, SEEK_SET);
    
    // le os 56 bytes que representam os bytes de enderecamento e apaga o 55th bit
    unsigned long page_frame_number = 0;
	fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);
    page_frame_number &= 0x7FFFFFFFFFFFFF;				// 

    fclose(pagemap);
    return page_frame_number;
}

int main()
{
	const char *name = "shared_memory";
	const int SIZE = 4096;

	int shm_fd;
	void *ptr;
	int i;

	shm_fd = shm_open(name, O_RDONLY, 0666);
	ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);

	printf("%s\n",(char*)ptr);

	printf("Endereco Logico: %p\n", ptr);
	unsigned long int numeroPageFrame = getNumeroPageFrame(ptr);
	printf("Endereco de pagina: %lu\n", numeroPageFrame);
	// encontra a distancia entre o ponteiro e o inicio da pagina
	unsigned int distancia = (unsigned long)ptr % getpagesize();
	unsigned long int enderecoFisico = numeroPageFrame << PAGE_SHIFT + distancia;
	printf("Endereco Fisico do Quadro: %lu\n", enderecoFisico);

	return 0;
}
