#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <omp.h>

bool is_perfect(int n) {
    int divisors_sum = 1; // Починаємо з 1, тому що 1 є дільником для всіх цілих чисел і не входить у цикл нижче.
    if (n < 2) return false; // Якщо n менше 2, воно не може бути досконалим числом, тому одразу повертаємо false.

    // Перебираємо можливі дільники числа n, починаючи з 2 (оскільки 1 вже враховано у divisors_sum) до n/2 (включно).
    // Жоден дільник числа n не може бути більшим за n/2 (крім самого числа n, яке не вважається правильним дільником).
    for (int i = 2; i <= n / 2; i++) {
        if (n % i == 0) { // Якщо i є дільником n...
            divisors_sum += i; // ...додаємо i до суми дільників.
        }
    }

    return divisors_sum == n; // Число є досконалим, якщо сума його правильних дільників дорівнює самому числу.
}


std::vector<int> find_perfect_numbers(const std::vector<int>& numbers, bool parallel, int num_threads = 4) {
    std::vector<int> perfectNumbers; // Вектор для зберігання всіх досконалих чисел

    if (parallel) { // Якщо true - виконується паралельний алгоритм
        omp_set_num_threads(num_threads); // Задається кількість паралельних потоків для OpenMP

#pragma omp parallel for shared(perfectNumbers) // Розпаралелювання цику за допомогою OpenMP. Вектор perfectNumbers є спільним для потоків.
        for (int i = 0; i < numbers.size(); i++) { // Перебір всіх чисел у вхідному векторі.
            if (is_perfect(numbers[i])) {
#pragma omp critical // Використання критичної секції для безпечного додавання досконалого числа до вектора, оскільки вектор не є потокобезпечним.
                perfectNumbers.push_back(numbers[i]);
            }
        }
    }
    else { // Послідовний режим
        for (int num : numbers) {
            if (is_perfect(num)) { 
                perfectNumbers.push_back(num); 
            }
        }
    }

    return perfectNumbers;
}


int main() {
    std::vector<int> numbers;
    // Завантажуємо вхідні числа
    std::ifstream inputFile("input.txt");
    int temp;
    while (inputFile >> temp) {
        numbers.push_back(temp);
    }
    inputFile.close();

    // Запуск виконання в паралельному режимі
    auto startParallel = std::chrono::high_resolution_clock::now();
    auto perfectNumbersParallel = find_perfect_numbers(numbers, true, 4); // Задана кількість потоків
    auto endParallel = std::chrono::high_resolution_clock::now();

    // Запуск виконання в послідовному режимі
    auto startSequential = std::chrono::high_resolution_clock::now();
    auto perfectNumbersSequential = find_perfect_numbers(numbers, false);
    auto endSequential = std::chrono::high_resolution_clock::now();

    // Підрахунок та виведення на екран витраченого часу на паралельне та послідовне виконання
    std::chrono::duration<double, std::milli> elapsedParallel = endParallel - startParallel;
    std::chrono::duration<double, std::milli> elapsedSequential = endSequential - startSequential;

    std::cout << "Parallel elapsed time: " << elapsedParallel.count() << " ms\n";
    std::cout << "Sequential elapsed time: " << elapsedSequential.count() << " ms\n";

    // Збереження досконалих чисел у файл
    std::ofstream outputFile("output.txt");
    for (int num : perfectNumbersParallel) {
        outputFile << num << std::endl;
    }
    outputFile.close();
}
