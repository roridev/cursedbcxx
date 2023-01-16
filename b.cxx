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

// TODO: Remover os usos da lib Boost. Usar só stdlib.
#include <boost/random/uniform_int_distribution.hpp>
#include <random>
#include <sys/types.h>

#include <utility>

// Cria um buffer circular com tamanho 10
boost::circular_buffer<u_int64_t> bcd_buffer (10);

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

bool flag = false;
bool isValidBinaryNumber = false;
u_int64_t input;
u_int64_t SHARED_MOD_REGISTER = 0;

void populate() {
    int count = 10;
    while(count != 0) {
        int num = dist(gen);
        bcd_buffer.push_front(num);
        count--;
    }
}

/*
 * Time to go bogo!
 */

void runUntilMatch(u_int64_t match) {
    int count = 10;
    while (bcd_buffer.front() != match || count != 0) {
        bcd_buffer.pop_front();
        count--;
    }

    if (count == 0) {
        populate();
        runUntilMatch(match);
    }
}

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
    u_int64_t current_digit = 0;
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
        barretMod(valor, 10);
        current_digit = SHARED_MOD_REGISTER;
        flags.push_front((current_digit & 14) == 0);
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


        barretMod(valor, 2);
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

        if (current_pwr != 0) { valor -= (1 << (current_pwr - 1)); } else { valor -= 1; }

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
