

#include "cpp_concurrency_code.h"
#include <cstdlib>




template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		T data;
		node* next;

		node(T const& _d) : data(_d) {}
	};

	std::atomic<node*> head;

public:

	void push(T const& data)
	{
		node* const nd = new node(data);
		nd->next = head.load();
		while (!head.compare_exchange_weak(nd->next, nd));
		// 不相等的话， nd->next 会被赋予 head 的值。
	}

	void pop(T& result)
	{
		node* old = head.load();
		while (!head.compare_exchange_weak(old, old->next));
		result = old->next;
	}

};




template<typename T>
class lock_free_stack_2
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		node* next;

		node(T const& _d) : data(std::make_shared<T>(_d)) {}
	};

public:
	void push(T const& data)
	{
		node* const nd = new node(data);
		nd->next = head.load();
		while (!head.compare_exchange_weak(nd->next, nd));
	}

	std::shared_ptr<T> pop()
	{
		node* old = head.load();
		while (old &&
			!head.compare_exchange_weak(old, old.next));
		return old ? old->data : std::shared_ptr<T>();
	}
};






// 这个不完整，没有 node 的定义。 head 的定义。
template<typename T>
class lock_free_stack_3
{
private:
	std::atomic<unsigned> th_in_pop;	// thread in pop
	void try_reclaim(node* old);

public:
	std::shared_ptr<T> pop()
	{
		++th_in_pop;		// ..没有看到 --的地方。 书上是：在 reclaim 中 --

		node* old = head.load();
		while (old &&
			!head.compare_exchange_weak(old, old->next));
		std::shared_ptr<T> res;
		if (old)
			res.swap(old->data);

		try_reclaim(old);
		return res;
	}
};


template<typename T>
class lock_free_stack_4
{
private:
	std::atomic<node*> to_be_deleted;

	static void delete_nodes(node* nodes)
	{
		while (nodes)
		{
			node* nxt = nodes->next;
			delete nodes;
			nodes = nxt;
		}
	}

	void try_recliam(node* old_head)
	{
		if (threads_in_pop == 1)
		{
			node* nodes_to_delete = to_be_deleted.exchange(nullptr);
			if (!--threads_in_pop)
			{
				delete_nodes(nodes_to_delete);
			}
			else if (nodes_to_delete)
			{
				chain_pending_nodes(nodes_to_delete);
			}
			delete old_head;
		}
		else
		{
			chain_pending_node(old_head);
			--threads_in_pop;
		}
	}

	void chain_pending_nodes(node* nodes)
	{
		node* lst = nodes;
		while (node* const nxt = lst->next)
		{
			lst = nxt;
		}
		chain_pending_nodes(nodes, lst);
	}

	void chain_pending_nodes(node* first, node* last)
	{
		last->next = to_be_deleted;
		while (!to_be_deleted.compare_exchange_weak(last->next, first));
	}

	void chain_pending_node(node* n)
	{
		chain_pending_nodes(n, n);
	}

};




template<typename T>
class lock_free_stack_6
{

public:

	std::shared_ptr<T> pop()
	{
		std::atomic<void*>& hp = get_hazard_pointer_for_current_thread();
		node* old = head.load();
		node* tmp;
		do
		{
			tmp = old;
			hp.store(old);
			old = head.load();
		} while (old != tmp);
		
		while (old && !head.compare_exchange_strong(old, old->next));

		hp.store(nullptr);
		std::shared_ptr<T> res;
		if (old)
		{
			res.swap(old->data);
			if (outstanding_hazard_pointers_for(old))
			{
				recliam_later(old);
			}
			else
			{
				delete old;
			}
			delete_nodes_with_no_hazards();
		}
		return res;
	}
};






template<typename T>
class lock_free_stack_7
{
private:
	struct node
	{
		std::shared_ptr<T> data;
		std::shared_ptr<node> next;
		node(T const& _d) : data(std::make_shared<T>(_d)) {}
	};
	std::shared_ptr<node> head;

public:
	void push(T const& data)
	{
		std::shared_ptr<node> const nd = std::make_shared<node>(data);
		nd->next = head.load();
		while (!std::atomic_compare_exchange_weak(&head, &nd->next, nd));
	}
	std::shared_ptr<T> pop()
	{
		std::shared_ptr<node> old = std::atomic_load(&head);
		while (old && !std::atomic_compare_exchange_weak(&head, &old, old->next));
		if (old)
		{
			std::atomic_store(&old->next, std::shared_ptr<node>());
			return old->data;
		}
		return std::shared_ptr<T>();
	}
	~lock_free_stack_7()
	{
		while (pop());
	}
};








template<typename T>
class lock_free_stack_9
{
private:
	struct node;
	struct counted_node_ptr
	{
		int external_count;
		node* ptr;
	};
	struct node
	{
		std::shared_ptr<T> data;
		std::atomic<int> internal_count;
		counted_node_ptr next;

		node(T const& _d) : data(std::make_shared<T>(_d)), internal_count(0) {}
	};
	std::atomic<counted_node_ptr> head;

public:
	~lock_free_stack()
	{
		while (pop());
	}

	void push(T const& data)
	{
		counted_node_ptr nd;
		nd.ptr = new node(data);
		nd.external_count = 1;
		nd.ptr->next = head.load();
		while (!head.compare_exchange_weak(nd.ptr->next, nd));
	}

};
















int main()
{



	//std::this_thread::sleep_for()

	system("PAUSE");

	return 0;
}