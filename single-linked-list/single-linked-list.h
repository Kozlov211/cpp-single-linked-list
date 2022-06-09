#pragma once
#include <cstddef>
#include <string>
#include <utility>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>

using namespace std;

template <typename Type>
class SingleLinkedList {
private:
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& val, Node* next)
		    : value(val)
		    , next_node(next) {
		}
		Type value;
		Node* next_node = nullptr;
	};

	// Шаблон класса «Базовый Итератор».
	// Определяет поведение итератора на элементы односвязного списка
	// ValueType — совпадает с Type (для Iterator) либо с const Type (для ConstIterator)
	template <typename ValueType>
	class BasicIterator {
	private:
		// Класс списка объявляется дружественным, чтобы из методов списка
		// был доступ к приватной области итератора
		friend class SingleLinkedList;
		Node* node_ = nullptr;
		
		// Конвертирующий конструктор итератора из указателя на узел списка
		explicit BasicIterator(Node* node) : node_(node) { }
	public:
		// Объявленные ниже типы сообщают стандартной библиотеке о свойствах этого итератора

		// Категория итератора — forward iterator
		// (итератор, который поддерживает операции инкремента и многократное разыменование)
		using iterator_category = std::forward_iterator_tag;
		// Тип элементов, по которым перемещается итератор
		using value_type = Type;
		// Тип, используемый для хранения смещения между итераторами
		using difference_type = std::ptrdiff_t;
		// Тип указателя на итерируемое значение
		using pointer = ValueType*;
		// Тип ссылки на итерируемое значение
		using reference = ValueType&;

		BasicIterator() = default;

		// Конвертирующий конструктор/конструктор копирования
		// При ValueType, совпадающем с Type, играет роль копирующего конструктора
		// При ValueType, совпадающем с const Type, играет роль конвертирующего конструктора
		BasicIterator(const BasicIterator<Type>& other) noexcept {
			(*this).node_ = other.node_;
		}

		// Чтобы компилятор не выдавал предупреждение об отсутствии оператора = при наличии
		// пользовательского конструктора копирования, явно объявим оператор = и
		// попросим компилятор сгенерировать его за нас
		BasicIterator& operator=(const BasicIterator& rhs) = default;

		// Оператор сравнения итераторов (в роли второго аргумента выступает константный итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return ((&(this->node_->value) == &(rhs.node_->value)) ? true : false);
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		// Оператор сравнения итераторов (в роли второго аргумента итератор)
		// Два итератора равны, если они ссылаются на один и тот же элемент списка либо на end()
		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return (this->node_ == rhs.node_ ? true : false); 
		}

		// Оператор проверки итераторов на неравенство
		// Противоположен !=
		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return !(*this == rhs);
		}

		// Оператор прединкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает ссылку на самого себя
		// Инкремент итератора, не указывающего на существующий элемент списка, приводит к неопределённому поведению
		BasicIterator& operator++() noexcept {
			assert(node_ != nullptr);
			this->node_ = this->node_->next_node;
			return *this;
		}

		// Оператор постинкремента. После его вызова итератор указывает на следующий элемент списка
		// Возвращает прежнее значение итератора
		// Инкремент итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		BasicIterator operator++(int) noexcept {
			auto old_value(*this);
			++(*this);
			return old_value;
		}

		// Операция разыменования. Возвращает ссылку на текущий элемент
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] reference operator*() const noexcept {
			assert(node_ != nullptr);
			return this->node_->value;
		}

		// Операция доступа к члену класса. Возвращает указатель на текущий элемент списка
		// Вызов этого оператора у итератора, не указывающего на существующий элемент списка,
		// приводит к неопределённому поведению
		[[nodiscard]] pointer operator->() const noexcept {
			assert(node_ != nullptr);
			Type* ptr_value = &(this->node_->value);
			return ptr_value;
		}
	};

public:
	SingleLinkedList() {
		size_ = 0;
		end_ = Iterator(head_.next_node);
		const_end_ = ConstIterator(head_.next_node);
	}

	SingleLinkedList(std::initializer_list<Type> values) {
		Iterator it = before_begin();
		for (auto value : values) {
			InsertAfter(it, value);
			it++;
		}
	}

	SingleLinkedList(const SingleLinkedList& other) {
		assert(size_ == 0 && head_.next_node == nullptr);
		SingleLinkedList tmp;
		Iterator it = before_begin();
		for (auto value : other) {
			InsertAfter(it, value);
			it++;
		}	
		swap(tmp);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (!(this == &rhs)) {
		    SingleLinkedList tmp(rhs);
		    swap(tmp);
		}
		return *this;
	}

	// Обменивает содержимое списков за время O(1)
	void swap(SingleLinkedList& other) noexcept {
		swap(head_.next_node, other.head_.next_node);
		size_t temp_size = size_;
		size_ = other.size_;
		other.size_ = temp_size;
		
	}

	void PopFront() noexcept {
		assert(head_.next_node != nullptr);
		Node* tmp = head_.next_node;
		head_.next_node = tmp->next_node;
		delete tmp;
		--size_;
	}

	// Вставляет элемент value в начало списка за время O(1)
	void PushFront(const Type& value) {
		head_.next_node = new Node(value, head_.next_node);
		++size_;
	}

	~SingleLinkedList() {
		Clear();
	}

	// Очищает список за время O(N)
	void Clear() noexcept {
		Node* node = nullptr;
		while (head_.next_node) {
			node = head_.next_node;
			head_.next_node = (*head_.next_node).next_node;
			delete node;
		}
		size_ = 0;
	}

	// Возвращает количество элементов в списке за время O(1)
	[[nodiscard]] size_t GetSize() const noexcept {
		return size_;
	}

	// Сообщает, пустой ли список за время O(1)
	[[nodiscard]] bool IsEmpty() const noexcept {
		return (size_ != 0 ? false : true);
	}

	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	// Итератор, допускающий изменение элементов списка
	using Iterator = BasicIterator<Type>;
	// Константный итератор, предоставляющий доступ для чтения к элементам списка
	using ConstIterator = BasicIterator<const Type>;

	// Возвращает итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	[[nodiscard]] Iterator begin() noexcept {
		return (size_ > 0 ? Iterator(head_.next_node) : end());
	}

	// Возвращает итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator end() noexcept {
		// Реализуйте самостоятельно
		return end_;
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен end()
	// Результат вызова эквивалентен вызову метода cbegin()
	[[nodiscard]] ConstIterator begin() const noexcept {
		return cbegin();
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	// Результат вызова эквивалентен вызову метода cend()
	[[nodiscard]] ConstIterator end() const noexcept {
		return cend();
	}

	// Возвращает константный итератор, ссылающийся на первый элемент
	// Если список пустой, возвращённый итератор будет равен cend()
	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return (size_ > 0 ? ConstIterator(head_.next_node) : cend());
	}

	// Возвращает константный итератор, указывающий на позицию, следующую за последним элементом односвязного списка
	// Разыменовывать этот итератор нельзя — попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cend() const noexcept {
		return const_end_;
	}

	// Возвращает итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] Iterator before_begin() noexcept {
		return Iterator(&head_);
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		Node* head_ptr = const_cast<SingleLinkedList<int>::Node *>(&head_);
		return ConstIterator(head_ptr);
	}

	// Возвращает константный итератор, указывающий на позицию перед первым элементом односвязного списка.
	// Разыменовывать этот итератор нельзя - попытка разыменования приведёт к неопределённому поведению
	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return cbefore_begin();
	}

	// Вставляет элемент value после элемента, на который указывает pos.
	// Возвращает итератор на вставленный элемент
	// Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
	Iterator InsertAfter(ConstIterator pos, const Type& value) {     
		assert(pos.node_ != nullptr);
		Node* new_node = new Node(value, pos.node_->next_node);
		pos.node_->next_node = new_node;
		++size_;
		return Iterator(new_node);
	}

	//Удаляет элемент, следующий за pos.
	// Возвращает итератор на элемент, следующий за удалённым
	Iterator EraseAfter(ConstIterator pos) noexcept {
		assert(pos.node_ != nullptr);
		Node* delete_node = pos.node_->next_node;
		pos.node_->next_node = delete_node->next_node;
		--size_;
		delete delete_node;
		return Iterator(pos.node_->next_node);
	}

private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_ = 0;
	Iterator end_;
	ConstIterator const_end_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs || lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs <= rhs);
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs < rhs);
} 

