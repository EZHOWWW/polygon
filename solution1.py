import numpy as np
import numpy.typing as npt


class Solution1:
    @staticmethod
    def max_even_sequence(arr: npt.NDArray[np.int_]) -> int:
        """
        Найти самую длинную последовательность чётных чисел
        Очень простой пример (наверное попозже сделаю что то посложнее :))
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
