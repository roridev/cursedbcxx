#include <list>
#include <vector>
#include <iostream>
#include <random>

typedef std::list<bool> BitFlag;

std::uniform_int_distribution<> dist(0, (10 - 1));
std::random_device gen;

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

uint64_t input;
bool SHARED_FLAG = false;
uint64_t SHARED_MOD_REGISTER = 0;
uint64_t SHARED_INDEX_REGISTER = 0;
uint64_t SHARED_ACC_REGISTER = 0;
BitFlag SHARED_BITFLAG;

void barretMod(uint64_t x, uint64_t y) {
    SHARED_MOD_REGISTER = x - (((int) floor((x/(double)y)))*y);
}

void __dec2bin (uint64_t x) {
    SHARED_ACC_REGISTER = 0;
    SHARED_INDEX_REGISTER = 0;
    SHARED_FLAG = false;

    SHARED_BITFLAG.clear();

    while(x != 0) {
        while ((uint64_t) (1 << SHARED_ACC_REGISTER) <= x) {
            if (!SHARED_FLAG) SHARED_BITFLAG.push_front(false);
            SHARED_ACC_REGISTER++;
        }
        SHARED_FLAG = true;

        SHARED_INDEX_REGISTER = 1;

        while (SHARED_ACC_REGISTER != 1) {
            SHARED_INDEX_REGISTER <<= 1;
            SHARED_ACC_REGISTER--;
        }

        while((uint64_t) (1 << SHARED_ACC_REGISTER) <= x) {
            SHARED_ACC_REGISTER++;
        }

        x -= SHARED_INDEX_REGISTER;

        SHARED_INDEX_REGISTER = 0;

        for (BitFlag::reverse_iterator r = SHARED_BITFLAG.rbegin(); r != SHARED_BITFLAG.rend(); r++) {
            if (SHARED_INDEX_REGISTER == SHARED_ACC_REGISTER - 1 ) *r = true;
            SHARED_INDEX_REGISTER++;
        }

        SHARED_ACC_REGISTER = 0;
        SHARED_INDEX_REGISTER = 0;
    }

    SHARED_FLAG = false;
}

void __binDec2Bin(uint64_t x) {
    SHARED_BITFLAG.clear();
    while(x != 0) {
        barretMod(x, (1<<1));
        SHARED_BITFLAG.push_front(SHARED_MOD_REGISTER == 1);
        x /= 10;
    }
    SHARED_MOD_REGISTER = 0;
}

void __bin2Dec() {
    SHARED_ACC_REGISTER = 0;
    SHARED_INDEX_REGISTER = 0;
    for (BitFlag::reverse_iterator x = SHARED_BITFLAG.rbegin(); x != SHARED_BITFLAG.rend(); x++) {
        uint64_t val = (uint64_t) *x;
        for (uint64_t i = 0; i < SHARED_INDEX_REGISTER; i++) {
            val <<= 1;
        }
        SHARED_ACC_REGISTER += val;
        SHARED_INDEX_REGISTER++;
    }

    SHARED_INDEX_REGISTER = 0;
    SHARED_BITFLAG.clear();
}

void menu() {
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

void validateBinary(uint64_t valor) {
    SHARED_INDEX_REGISTER = 0;
    SHARED_BITFLAG.clear();
    SHARED_FLAG = false;
    while (valor != 0) {
        barretMod(valor, 10);
        SHARED_BITFLAG.push_front((SHARED_MOD_REGISTER & (0b111 << 1) ) == 0);
        valor /= 10;
        SHARED_INDEX_REGISTER++;
    }

    SHARED_FLAG = true;
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
