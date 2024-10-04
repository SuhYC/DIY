#pragma once

#include <atomic>

template <typename T>
class MyStack
{
public:
    MyStack()
    {
        top.store(nullptr);
    }

    ~MyStack()
    {
        Node* tmp = pop();
        while (tmp != nullptr)
        {
            delete tmp;
            tmp = pop();
        }
    }

    void Init(T data_)
    {
        for (int i = 0; i < 100; i++)
        {
            Node* tmp = new Node(data_);
            push(tmp);
        }
    }

    class Node
    {
    public:
        Node(T data_) : data(data_)
        {
            next = nullptr;
        }
        T data;
        Node* next;
    };

    void push(Node* data_)
    {
        Node* oldtop;
        Node* newtop;
        do
        {
            oldtop = top.load();
            data_->next = oldtop;
            newtop = addStamp(data_);
        } while (!top.compare_exchange_strong(oldtop, newtop));
    }

    Node* pop()
    {
        Node* oldHead;
        Node* newHead;
        Node* removeHead;
        do {
            oldHead = top.load();
            if (oldHead == nullptr)
            {
                return nullptr;
            }

            removeHead = removeStamp(oldHead);

            newHead = removeHead->next;
        } while (!top.compare_exchange_strong(oldHead, newHead));

        return removeHead;
    }

    inline Node* addStamp(Node* pNode_)
    {
        return reinterpret_cast<Node*>((count++ << 52) | reinterpret_cast<uintptr_t>(pNode_));
    }

    inline Node* removeStamp(Node* pNode_)
    {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(pNode_) & (0x000FFFFFFFFFFFFFU));
    }

    std::atomic<long long> count; // For prevent ABA.
    std::atomic<Node*> top;
};