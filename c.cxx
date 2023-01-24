/*
 * Esse código foi feito da maneira mais triste
 * humanamente possível.
 *
 * Criado em: 15/01/2023, 19h50m GMT-3
 * Finalizado em: 24/01/2023, 12h50m GMT-3
 *
 * Autoria: Sistema Alikind (roridev)
 *
 * Nota: Sim a gente decidiu usar c++ pra fazer um código absurdamente ineficiente
 * A ironia disso não foi perdida, essa era nossa intenção.
 */

#include <list>
#include <vector>
#include <iostream>
#include <random>


/*
 * Tipos utilizados:
 * uint64_t, bool, std::list<bool>
 */

typedef std::list<bool> BitFlag;

/*
 *  RNG para a UX divina
 */

std::uniform_int_distribution<> dist(0, (10 - 1));
std::random_device gen;

/*
 * Macrolândia para deixar o C++ "high level"
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


/*
 * Variáveis.
 */

uint64_t input;
bool SHARED_FLAG = false;
uint64_t SHARED_MOD_REGISTER = 0;
uint64_t SHARED_INDEX_REGISTER = 0;
uint64_t SHARED_ACC_REGISTER = 0;
BitFlag SHARED_BITFLAG;

// Barrett Modulo Reduction.
void barretMod(uint64_t x, uint64_t y) {
    SHARED_MOD_REGISTER = x - (((int) floor((x/(double)y)))*y);
}

/*
 * Logica para converter Decimal -> Binário
 * Nota: É ineficiente pra burro.
 */

void __dec2bin (uint64_t x) {
    /* Vamo ser diferentão nesse aqui.
     * A gente vai testar *todas as bases* até achar a que é maior que o numero
     * tirar ela e resetar o contador pra zero
     * e testar tudo denovo.
     *
     * 2^n  1010 (10) |
     *    0     1      |     0
     *    1    10      |    00
     *    2   100      |   000
     *    3  1000      |  0000
     *    4 10000 !!!! | 00000 -> 0000 -> 000 -> 1000 Overflow (A gente sabe que o bit 4 é 1, seta ele e reduz)
     *---------------------------
     *      0010 (2)
     *    0    1
     *    1   10
     *    2  100 !!!! Overflow (A gente sabe que o bit 2 é 1, seta ele e reduz)
     * ---------------------------
     *      0000
     */


    SHARED_ACC_REGISTER = 0;
    SHARED_INDEX_REGISTER = 0;
    SHARED_FLAG = false;

    SHARED_BITFLAG.clear();

    while(x != 0) {
        /* Esse loop simplesmente faz com que a SHARED_BITFLAG esteja com a quantidade
         * De zeros (false) corretas e conta no SHARED_ACC_REGISTER qual é a potencia de 2
         * (ou quantidade de bits) necessárias para representar x.
         */
        while ((uint64_t) (1 << SHARED_ACC_REGISTER) <= x) {
            if (!SHARED_FLAG) SHARED_BITFLAG.push_front(false);
            SHARED_ACC_REGISTER++;
        }
        SHARED_FLAG = true;

        // Seta o SHARED_INDEX_REGISTER pra 1 pq a gente vai usar isso
        // para pegar 2^n.
        SHARED_INDEX_REGISTER = 1;

        // Isso pega 2^n com left shifts no INDEX, e ao mesmo tempo reduz ACC para 0.
        // Basicamente invertendo ACC e INDEX para deixar ainda mais abiblublé das ideias.

        while (SHARED_ACC_REGISTER != 1) {
            SHARED_INDEX_REGISTER <<= 1;
            SHARED_ACC_REGISTER--;
        }

        // Isso retorna o ACC para o seu valor certo (é o msm código do começo)
        while((uint64_t) (1 << SHARED_ACC_REGISTER) <= x) {
            SHARED_ACC_REGISTER++;
        }

        // Tira a ultima potência de dois do x. (Bit positivo mais alto).
        x -= SHARED_INDEX_REGISTER;

        // Reseta o INDEX.
        SHARED_INDEX_REGISTER = 0;

        // Seta o Bit positivo mais alto do numero na bitflag.
        for (BitFlag::reverse_iterator r = SHARED_BITFLAG.rbegin(); r != SHARED_BITFLAG.rend(); r++) {
            if (SHARED_INDEX_REGISTER == SHARED_ACC_REGISTER - 1 ) *r = true;
            SHARED_INDEX_REGISTER++;
        }

        // Reseta os valores para a prox execução do loop.
        SHARED_ACC_REGISTER = 0;
        SHARED_INDEX_REGISTER = 0;
    }

    // Reseta o SHARED_FLAG, pq afinal, se a gente pudesse usar mais que um boolean
    // estouraria o orçamento.
    SHARED_FLAG = false;
}

/*
 * Converte um numero "binário" (representado como um uint64_t)
 * para uma BitFlag.
 *
 * Leia-se: Binário (Decimal) -> Binário
 */

void __binDec2Bin(uint64_t x) {
    SHARED_BITFLAG.clear();
    // Base 2 só tem 2 valores, 0 e 1.
    // No caso mais simples, 1 bit, você simplesmente checka se o numero é impar ou par.
    // Isso pega o valor do ultimo bit.
    while(x != 0) {
        barretMod(x, (1<<1));
        SHARED_BITFLAG.push_front(SHARED_MOD_REGISTER == 1);
        // Como o numero é decimal, temos que remover uma casa para pegar o proximo "bit".
        x /= 10;
    }

    // Ai você pergunta, porque resetar o MOD?
    // barretMod tem a resposta!
    SHARED_MOD_REGISTER = 0;
}

/*
 * Logica para converter Decimal -> Binário
 * Nota: Não parece ser tão ruim, mas isso precisa da função
 * __binDec2Bin para ter a BitFlag.
 */

void __bin2Dec() {
    SHARED_ACC_REGISTER = 0;
    SHARED_INDEX_REGISTER = 0;
    for (BitFlag::reverse_iterator x = SHARED_BITFLAG.rbegin(); x != SHARED_BITFLAG.rend(); x++) {
        // Pega o valor do bit (true/false) e converte em integer.
        uint64_t val = (uint64_t) *x;
        // Para cada numero da potência de 2, duplique o valor do bit.
        for (uint64_t i = 0; i < SHARED_INDEX_REGISTER; i++) {
            val <<= 1;
        }
        // Adiciona o valor de bit * (2^INDEX) em ACC.
        SHARED_ACC_REGISTER += val;
        // Aumenta o numero da potência de 2.
        SHARED_INDEX_REGISTER++;
    }

    SHARED_INDEX_REGISTER = 0;
    SHARED_BITFLAG.clear();
}

void menu() {
    // É aqui que os random são utilizados.
    // Para uma melhor experiência usando esse lindo software
    // os numeros do menu são randomizados :)

    uint64_t key_b2d = dist(gen);
    uint64_t key_d2b = dist(gen);
    uint64_t shuffle_count;

    while (key_d2b == key_b2d) {
        key_b2d = dist(gen);
    }

    println("Por favor digite a operação:");
    println(key_b2d, ": Converter de Binário (Base2) para Decimal (Base10)");
    println(key_d2b, ": Converter de Decimal (Base10) para Binário (Base2)");

    prompt(input, "> ");

    if(input == key_b2d) {
        SHARED_FLAG = true;
        println("Valor válido. Selecionando Binário -> Decimal.");
    } else if (input == key_d2b) {
        SHARED_FLAG = false;
        println("Valor válido. Selecionando Decimal -> Binário");
    } else {
        shuffle_count = dist(gen);
        println("Valor inválido. Reiniciando");

        for (uint64_t i = 0; i < shuffle_count; i++) {
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

// Mas ai que fica a pergunta, como que você interpreta um valor decimal como se fosse binário
// só se garantindo na aritmética, já que std::string é para os fracos?


// Temos que checkar se 'valor' é valido só usando aritmética!
void validateBinary(uint64_t valor) {
    // Nesse ponto, o numero tem que ser tratado como base 10 (porque na realidade, ele é um numero decimal.)
    // Base 10 possui 10 numeros: [0,1,2,3,4,5,6,7,8,9]
    // Desses, apenas dois nos interessa. 0 e 1.

    SHARED_INDEX_REGISTER = 0;
    SHARED_BITFLAG.clear();
    SHARED_FLAG = false;

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

    // Você (pensando parecendo um bobo): "Mas Alice, um simples maior quê 1 resolveria isso..."
    // Meu caro jovem, um simplório x > 1 você encontra em qualquer lugar, incluindo no Geeks4Geeks,
    // eu JAMAIS lhe traria algo proveniente de tais fontes abissáis de jimo.
    // Para a pergunta "Como eu detecto um numero que seja maior que um" a resposta claramente é
    // "Cheque o valor binário de todos os digitos, encontre uma bitmask que isole ela á algo comparável a zero
    // e compare com zero"

    // Para pegar a casa da unidade atual é trivial. Um simples x % 10.

    while (valor != 0) {
        // Pega a casa da unidade atual
        barretMod(valor, 10);
        // Não é magia, é tecnologia.
        SHARED_BITFLAG.push_front((SHARED_MOD_REGISTER & (0b111 << 1) ) == 0);
        // Remove uma casa decimal.
        // Atividade para o leitor: Qual é o plural de "decimal"?
        valor /= 10;
        SHARED_INDEX_REGISTER++;
    }

    SHARED_FLAG = true;
    // Depois de tudo aquilo é facil. Se tiver 1 (um) digito que não passou no teste
    // o numero é inválido.
    for (bool bit: SHARED_BITFLAG) {
        if(!bit) {
            println("Digito Inválido Detectado.");
            SHARED_FLAG = false;
            break;
        }
    }

    SHARED_BITFLAG.clear();
    SHARED_INDEX_REGISTER = 0;
}

void bin2dec (uint64_t valor) {
    __binDec2Bin(valor);
    __bin2Dec();

    println("Conversão de \'", valor ,"\' para Decimal concluida.");
    println("Resultado: ", SHARED_ACC_REGISTER);
}

void dec2bin(uint64_t valor) {
    __dec2bin(valor);

    println("Conversão de \'", valor, "\' para Binário concluida.");
    print("Resultado: ");

    for (bool b: SHARED_BITFLAG) {
        auto i = int(b);
        print(i);
    }

    std::cout << std::endl;
}

int main() {
    menu();
    println("Digite o valor desejado.");
    prompt(input, "> ");

    if (SHARED_FLAG) {
        validateBinary(input);
        if (SHARED_FLAG) bin2dec(input);
    } else {
        dec2bin(input);
    }
    return 0;
}
