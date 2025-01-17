import numpy as np
import numpy.typing as npt


class Solution1:
    @staticmethod
    def max_even_sequence_nonumpy(arr: npt.NDArray[np.int_]) -> int:
        """
        Найти самую длинную последовательность чётных чисел, наивное решение без numpy
        """
        max_length = 0
        current_length = 0

        for num in arr:
            if num % 2 == 0:  # если четно то найс
                current_length += 1
                max_length = max(max_length, current_length)
            else:  # иначе не найс
                current_length = 0

        return max_length

    @staticmethod
    def max_even_sequence(arr: np.ndarray) -> int:
        """
        Найти длину самой длинной последовательности четных чисел, решение через numpy
        :param arr: одномерный массив целых чисел
        :return: длина самой длинной последовательности четных чисел
        """
        #  Создание логического массива четности, переводим из bool в int
        even_mask = (arr % 2 == 0).view(np.int8)
        # Находим переходы от true и false, concatenate нужен что бы покрыть случаи когда граница последовательности на границе массива
        changes = np.diff(np.concatenate(([0], even_mask, [0])))

        # Индексы начала и конца последовательностей из True
        start_indices = np.where(changes == 1)[0]
        end_indices = np.where(changes == -1)[0]
        lengths = end_indices - start_indices

        # Вернем максимальную из них (или 0, если таких нет)
        return lengths.max() if lengths.size > 0 else 0
