/*
 * Esse código foi feito da maneira mais triste
 * humanamente possível.
 *
 * Criado em: 15/01/2023, 19h50m GMT-3
 * Finalizado em:
 *
 * Autoria: Sistema Alikind (roridev)
 *
 * Nota: Sim a gente decidiu usar c++ pra fazer um código absurdamente ineficiente
 * A ironia disso não foi perdida, essa era nossa intenção.
 */

#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <iterator>

#include <stdio.h>

// Precisamos de buffer circular

// TODO: Remover os usos da lib Boost. Usar só stdlib.
#include <boost/circular_buffer.hpp>
// E uma double-linked list, pq não?
#include <list>
#include <vector>

// TODO: Remover os usos da lib Boost. Usar só stdlib.
#include <boost/random/uniform_int_distribution.hpp>
#include <random>
#include <sys/types.h>

#include <utility>

// Cria um buffer circular com tamanho 10

typedef std::list<bool> BitFlag;

// RNG
boost::random::uniform_int_distribution<> dist(0, 9);
std::random_device gen;


/*
 * Macrolândia
 */

template <typename T, typename... B> void print(T& t, B&... b) {
    std::cout  << std::forward<T>(t);
    using expander = int[];
    (void) expander {0, (void(std::cout << std::forward<B>(b)), 0)...};
}

template <typename T, typename... B> void println(T& t, B&... b) {
    print(t, b...);
    std::cout << std::endl;
}

template <typename T, typename... B> void prompt(T& t, B&... b) {
    print(b...);
    std::cin >> t;
}

template <typename T> void print_arr(T& t) {
    int idx = 0;
    std::cout << "[";
    for (auto a: t) {
        std::cout << a;
        if(idx != t.size() - 1) {
            std::cout << ",";
        }
        idx++;
    }
    std::cout << "]";
}


bool flag = false;
bool isValidBinaryNumber = false;
u_int64_t input;
u_int64_t SHARED_MOD_REGISTER = 0;
u_int64_t SHARED_WIDTH_REGISTER = 0;
u_int64_t SHARED_INDEX_REGISTER = 0;
u_int64_t SHARED_ACC_REGISTER = 0;
BitFlag SHARED_BITFLAG;

std::vector<u_int64_t> splitted;

// NOTE: https://eprint.iacr.org/2014/755.pdf

/*
 * "Modern Intel processors have implemented the modulo operation in the form
 * of a single division instruction.  This instruction will return both the quotient
 * and remainder in different registers. Therefore the modulo operation can be
 * computed in a single clock cycle"
 */
void barretMod(u_int64_t x, u_int64_t y) {
    // Risada maligna
    SHARED_MOD_REGISTER = x - (((int) floor((x/(double)y)))*y);
}

void calculate_width(u_int64_t alpha) {
    // Code like its ASM!!!!
    SHARED_INDEX_REGISTER = 0;    // EAX time!!!
    while (1 << SHARED_INDEX_REGISTER <= alpha) {
        SHARED_INDEX_REGISTER++;
    }
}

// Esse algoritmo para computar módulo requer converter
// O valor em binário e dividir em pedacinhos.

// Ou seja, cada uso de `ko_willMod` requer uma conversão em binário.

// Lindo demais.

// Como o unico módulo para converter em binário utilizando meu método
// é mod 2, teremos que descobrir um jeito de fazer mod 2 sem usar módulo.
// Caso contrário, o programa nunca termina.
void split(u_int64_t alpha, u_int64_t omega) {
    u_int64_t old_alpha = alpha;

    SHARED_ACC_REGISTER = 0; //Current Power
    SHARED_INDEX_REGISTER = 0; //Current Index

    SHARED_BITFLAG.clear(); // Bits

    bool populated = false;

    while (alpha != 0) {
        while ((1 << SHARED_ACC_REGISTER) <= alpha) {
            if (!populated) { SHARED_BITFLAG.push_front(false); }
            SHARED_ACC_REGISTER++;
        }

        populated = true;

        alpha -= (SHARED_ACC_REGISTER != 0) ? (1 << ( SHARED_ACC_REGISTER - 1)) : 1;

         for (BitFlag::reverse_iterator rev_iter = SHARED_BITFLAG.rbegin(); rev_iter != SHARED_BITFLAG.rend(); rev_iter++) {
            if (SHARED_INDEX_REGISTER == SHARED_ACC_REGISTER - 1) {
                *rev_iter = true;
            }
            SHARED_INDEX_REGISTER++;
        }

        SHARED_ACC_REGISTER = 0;
        SHARED_INDEX_REGISTER = 0;
    }

    BitFlag acc;


    /// 100011
    //       ^ [1]
    //      ^  [11]
    //     ^   [011]
    //    ^    [0011]

    // ------- [[0011]], []
    //   ^     [[0011]], [0]
    //  ^      [[0011]], [10]
    //-----------------------
    //         [[0011],[0010]]

    for (BitFlag::reverse_iterator rev_iter = SHARED_BITFLAG.rbegin(); rev_iter != SHARED_BITFLAG.rend(); rev_iter++) {
        acc.push_front(*rev_iter);
        if(acc.size() == omega) {
            SHARED_INDEX_REGISTER = 0;
            for (BitFlag::reverse_iterator rev_acc = acc.rbegin(); rev_acc != acc.rend(); rev_acc++) {
                SHARED_ACC_REGISTER += (1 << SHARED_INDEX_REGISTER) * int(*rev_acc);
                SHARED_INDEX_REGISTER++;
            }
            splitted.push_back(SHARED_ACC_REGISTER);
            acc.clear();
        }
    }

    if (acc.size() != 0) {
        SHARED_INDEX_REGISTER = 0;
        for (BitFlag::reverse_iterator rev_acc = acc.rbegin(); rev_acc != acc.rend(); rev_acc++) {
            SHARED_ACC_REGISTER += (1 << SHARED_INDEX_REGISTER) * int(*rev_acc);
            SHARED_INDEX_REGISTER++;
        }
        splitted.push_back(SHARED_ACC_REGISTER);
        acc.clear();
    }

    SHARED_INDEX_REGISTER = 0;
    SHARED_ACC_REGISTER = 0;
}


void ko_willMod(u_int64_t x, u_int64_t y) {
    calculate_width(y);
    split(x, SHARED_INDEX_REGISTER);
    calculate_width(y);

    u_int64_t n = splitted.size() - 1;
    barretMod(((1 << 1) << SHARED_INDEX_REGISTER), y);

    while (n > 0) {
        auto t = splitted[n];
        for (u_int64_t i = SHARED_INDEX_REGISTER; i > 0; i--) {
            t = t << 1;
            while ((t & (1 << SHARED_INDEX_REGISTER)) == 1) {
                t &= ~(1 << SHARED_INDEX_REGISTER);
                t += SHARED_MOD_REGISTER;
            }
        }
        splitted[n-1] += t;
        while (((splitted[n-1]) & (1 << SHARED_INDEX_REGISTER)) == 1) {
            splitted[n-1] &= ~(1 << SHARED_INDEX_REGISTER);
            splitted[n-1] += SHARED_MOD_REGISTER;
        }
        n--;
    }

    while (splitted[0] > y) {
        splitted[0] -= y;
    }

    SHARED_INDEX_REGISTER = 0;
    SHARED_MOD_REGISTER = splitted[0];
}



void menu() {

    /*
     * UX is my passion
     */

    u_int64_t key_b2d = dist(gen);
    u_int64_t key_d2b = dist(gen);

    while (key_d2b == key_b2d) {
        key_b2d = dist(gen);
    }

    uint64_t input;
    uint64_t shuffle_count;

    // #import <python>
    println("Por favor digite a operação:");
    println(key_b2d, ": Converter de Binário (Base2) para Decimal (Base10)");
    println(key_d2b, ": Converter de Decimal (Base10) para Binário (Base2)");

    prompt(input, "> ");

    if(input == key_b2d) {
        flag = true;
        println("Valor válido. Selecionando Binário -> Decimal.");
    } else if (input == key_d2b) {
        flag = false;
        println("Valor válido. Selecionando Decimal -> Binário");
    }
    else {
        shuffle_count = dist(gen);
        println("Valor inválido. Reiniciando");

        for (int i = 0; i < shuffle_count; i++) {
            dist(gen);
        }

        menu();
    }


}

// Aqui a gente lida com *números*
// String existe pra quem não se garante na aritmética.
// Ou seja, o numero binário chega como um valor em decimal, e a gente tem que interpretar
// ele como se fosse binário.

// Isso tem a "vantagem" que leading zero simplesmente morre.
// Para o programa "000001010" é a mesma coisa que dez mil e dez, apesar de ser somente 10 em decimal.

// Lindo demais né?

// Mas ai que fica a pergunta, como caralhos você interpreta um valor decimal como se fosse binário
// só se garantindo na aritmética, já que std::string é para os fracos?


// Temos que checkar se 'valor' é valido só usando aritmética!
void validateBinary(u_int64_t valor) {
    // Nesse ponto, o numero tem que ser tratado como base 10 (porque na realidade, ele é um numero decimal.)
    // Base 10 possui 10 numeros: [0,1,2,3,4,5,6,7,8,9]
    // Desses, apenas dois nos interessa. 0 e 1.
    u_int64_t current_pwr = 0;
    BitFlag flags;

    // A ideia pra checkar isso é simples
    // Tomemos por exemplo o numero mil e dez: 1010.
    // m c d u
    // 1 0 1 0

    // Como se checka a casa da unidade?
    // Nós precisamos de uma operação que siga essa tabela da verdade

    // 0000 | 1   0101 | 0
    // 0001 | 1   0110 | 0
    // 0010 | 0   0111 | 0
    // 0011 | 0   1000 | 0
    // 0100 | 0   1001 | 0

    // Percebe-se, que um valor que tenha quaisquer bits que não
    // seja o primeiro sendo verdadeiro/falso é inválido

    // Façamos então valor & b1110
    // 0 & 14 = 0000   5 & 14 = 0100
    // 1 & 14 = 0000   6 & 14 = 0110
    // 2 & 14 = 0010   7 & 14 = 0110
    // 3 & 14 = 0010   8 & 14 = 1000
    // 4 & 14 = 0100   9 & 14 = 1000

    // Logo (valor & b1110) == 0 nos dá a tabela-verdade desejada!

    // Para pegar a casa da unidade atual é trivial. Um simples x % 10.

    while (valor != 0) {
        ko_willMod(valor, 10);
        flags.push_front((SHARED_MOD_REGISTER & (0b111 << 1) ) == 0);
        valor /= 10;
        current_pwr++;
    }

    isValidBinaryNumber = true;

    for (bool bit: flags) {
        if(!bit) {
            println("Digito Inválido Detectado.");
            isValidBinaryNumber = false;
            break;
        }
    }
}

// Essa função assume que o valor passado para ela é um valor binário válido.
// Se vc mandar 21 pra ela eu não sei o que ela faz.
void bin2dec (u_int64_t valor) {
    uint64_t old_valor = valor;
    BitFlag flags;

    while (valor != 0) {
        // Base 2 só tem 2 valores, 0 e 1.
        // No caso mais simples, 1 bit, você simplesmente checka se o numero é impar ou par.
        // Isso pega o valor do ultimo bit.


        ko_willMod(valor, (1 << 1));
        flags.push_front(SHARED_MOD_REGISTER == 1);

        // Manda o shr decimal kkkj
        valor /= 10;
    }

    // No final temos todos os bits ordenadinhos na BitFlag.

    // Seria muito facil, eficiente e até trivial converter agora
    // usando Front -> Back com um acumulador que usa o tamanho da lista
    // pra gerar os valores.
    // acc += (2^(size - n)*it)

    // A lógica então é converter Back -> Front com dois acumuladores ao invéz
    // de apenas um.

    uint64_t acc = 0;
    uint64_t current_pwr = 0;

    for (BitFlag::reverse_iterator iter = flags.rbegin(); iter != flags.rend(); iter++) {
        uint64_t val = uint64_t(*iter);
        for (uint64_t i = 0; i < current_pwr; i++) {
            val <<= 1;
        }
        acc +=  val;
        current_pwr += 1;
    }

    println("Conversão de \'", old_valor ,"\' para Decimal concluida.");
    println("Resultado: ", acc);
}

void dec2bin(u_int64_t valor) {
    // Vamo ser diferentão nesse aqui.
    // A gente vai testar *todas as bases* até achar a que é maior que o numero
    // tirar ela e resetar o contador pra zero
    // e testar tudo denovo.

    // 2^n  1010 (10) |
    //   0     1      |     0
    //   1    10      |    00
    //   2   100      |   000
    //   3  1000      |  0000
    //   4 10000 !!!! | 00000 -> 0000 -> 000 -> 1000 Overflow (A gente sabe que o bit 4 é 1, seta ele e reduz)
    //---------------------------
    //     0010 (2)
    //   0    1
    //   1   10
    //   2  100 !!!! Overflow (A gente sabe que o bit 2 é 1, seta ele e reduz)
    //---------------------------
    //     0000

    uint64_t old_valor = valor;

    uint64_t current_pwr = 0;
    BitFlag bits;
    uint64_t current_idx = 0;


    bool populated = false;

    while (valor != 0){
        while ((1 << current_pwr) <= valor) {
            if (!populated) bits.push_front(false);
            current_pwr++;
        }

        populated = true;


        valor -= (current_pwr != 0) ? (1 << ( current_pwr - 1)) : 1;

        for (BitFlag::reverse_iterator rev_iter = bits.rbegin(); rev_iter != bits.rend(); rev_iter++) {
            if (current_idx == current_pwr - 1) {
                *rev_iter = true;
            }
            current_idx++;
        }

        current_pwr = 0;
        current_idx = 0;
    }

    println("Conversão de \'", old_valor, "\' para Binário concluida.");

    print("Resultado: ");

    for (bool b: bits) {
        auto i = int(b);
        print(i);
    }

    std::cout << std::endl;

}


// Sim, essa é a *unica* função que retorna algum valor.
// Global values FTW.

int main() {
    menu();
    println("Digite o valor desejado.");
    prompt(input, "> ");


    if (flag) {
        validateBinary(input);
        if (isValidBinaryNumber) bin2dec(input);
    } else {
        dec2bin(input);
    }
    return 0;
}
