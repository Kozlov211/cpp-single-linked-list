#pragma once

#include <string>

#include "single_linked_list.h"

using namespace single_linked_list;

namespace examples_use_single_linked_list {

void Contsructor() {
    SingleLinkedList<std::string> words1 {"the", "frogurt", "is", "also", "cursed"};
    SingleLinkedList<std::string> words2(words1); // {"the", "frogurt", "is", "also", "cursed"}
    SingleLinkedList<std::string> words3; // {}
}

void EqualsOperator() {
    SingleLinkedList<std::string> words1 {"the", "frogurt", "is", "also", "cursed"};
    SingleLinkedList<std::string> words2; // {}
    words2 = words1; // {"the", "frogurt", "is", "also", "cursed"}
}

void Swap() {
    SingleLinkedList<int> list1{1, 2, 3};
    SingleLinkedList<int> list2 {4, 5};
    list1.swap(list2); // list1{4,5} list2{1,2,3}
}

void PopFront() {
    SingleLinkedList<char> chars{'A', 'B', 'C', 'D'};
    chars.PopFront(); // chars{'B', 'C', 'D'}
}

void PushFront() {
    SingleLinkedList<std::string> letters; // {}
    letters.PushFront("abc"); // letters{"abc"}
}

void Clear() {
    SingleLinkedList<char> chars{'A', 'B', 'C', 'D'};
    chars.Clear(); // chars{}
}

void GetSize() {
    SingleLinkedList<double> list{1.2, 2.7};
    list.GetSize(); // 2
}

void IsEmpty() {
    SingleLinkedList<double> list{1.2, 2.7};
    list.IsEmpty(); // false
}

} // namespace examples_use_single_linked_list
