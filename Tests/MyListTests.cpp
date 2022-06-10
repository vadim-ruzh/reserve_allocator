#define BOOST_TEST_MODULE MyListTests

#include <boost/test/included/unit_test.hpp>
#include "../reserve_allocator/my_list.h"
#include "../reserve_allocator/reserve_allocator.h"

namespace utf = boost::unit_test;

int Factorial(int n)
{
	if(n == 0)
		return 1;

	return n * Factorial(n - 1);
}

int Fibonacci(int n)
{
	return n < 2 ? n : Fibonacci(n - 2) + Fibonacci(n - 1);
}

struct Hard {

	Hard(int fa_, int fi_) : fa(Factorial(fa_)), fi(Fibonacci(fi_))
	{
		
	}

	bool operator==(const Hard& other) const
	{
		return fa == other.fa && fi == other.fi;
	}
	bool operator==(Hard&& other) const
	{
		return fa == other.fa && fi == other.fi;
	}

	bool operator!=(const Hard& other) const
	{
		return !(fa == other.fa && fi == other.fi);
	}

	int fa;
	int fi;
};

using TestType = Hard;


BOOST_AUTO_TEST_SUITE(TestMyListWithStdAllocator)

BOOST_AUTO_TEST_SUITE(TestInsertInList)

	BOOST_AUTO_TEST_CASE(Insert)
	{
		constexpr int controlSize = 10;
		const TestType controlFrontValue{ 1,1 };
		const TestType controlBackValue{ controlSize,controlSize };

		auto testObject = my::List<TestType, std::allocator<TestType>>();
		for(int i = 1; i <= controlSize; ++i)
		{
			testObject.insert(testObject.end(), TestType{ i,i });
		}

		BOOST_CHECK(testObject.back() == controlBackValue);
		BOOST_CHECK(testObject.front() == controlFrontValue);
	}

	BOOST_AUTO_TEST_CASE(InsertMyltiply)
	{
		
		constexpr int controlSize = 10;
		const TestType controlValue{ 5,6 };
		auto testObject = my::List<TestType, std::allocator<TestType>>();


		testObject.insert(testObject.begin(), controlSize, controlValue);

		for (auto elem : testObject)
		{
			BOOST_CHECK(elem == controlValue);
		}

		BOOST_CHECK(testObject.size() == controlSize);
	}

	BOOST_AUTO_TEST_CASE(PushBack)
	{
		const TestType ControlValue{ 4,3 };
		auto testObject = my::List<TestType, std::allocator<TestType>>();

		testObject.push_back(ControlValue);
		BOOST_CHECK(testObject.back() == ControlValue);
	}

	BOOST_AUTO_TEST_CASE(PushFront)
	{
		const TestType ControlValue{ 5,7 };
		auto testObject = my::List<TestType, std::allocator<TestType>>();

		testObject.push_front(ControlValue);
		BOOST_CHECK(testObject.front() == ControlValue);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(TestRemoveFromList)

	BOOST_AUTO_TEST_CASE(Erase)
	{
		constexpr int controlSize = 5;
		const TestType controlBackValue{ controlSize - 1,controlSize - 1 };

		auto testObject = my::List<TestType, std::allocator<TestType>>();
		for (int i = 1; i <= controlSize; ++i)
		{
			testObject.insert(testObject.end(), TestType{ i,i });
		}

		testObject.erase(--testObject.end());

		BOOST_CHECK(testObject.back() == controlBackValue);
		BOOST_CHECK(testObject.size() == controlSize - 1);
	}

	BOOST_AUTO_TEST_CASE(EraseMultiply)
	{
		constexpr int controlSize = 5;
		const TestType controlFrontValue{ 1,1};

		auto testObject = my::List<TestType, std::allocator<TestType>>();
		for (int i = 1; i <= controlSize; ++i)
		{
			testObject.insert(testObject.end(), TestType{ i,i });
		}

		testObject.erase(++testObject.begin(), testObject.end());

		BOOST_CHECK(testObject.front() == controlFrontValue);
		BOOST_CHECK(testObject.size() == 1);
	}

	BOOST_AUTO_TEST_CASE(PopBack)
	{
		const TestType ControlValue{ 4,3 };
		auto testObject = my::List<TestType, std::allocator<TestType>>();

		testObject.push_back(ControlValue);
		testObject.pop_back();
		
		BOOST_CHECK(testObject.empty());
	}

	BOOST_AUTO_TEST_CASE(PopFront)
	{
		const TestType ControlValue{ 4,3 };
		auto testObject = my::List<TestType, std::allocator<TestType>>();

		testObject.push_front(ControlValue);
		testObject.pop_front();

		BOOST_CHECK(testObject.empty());
	}

	BOOST_AUTO_TEST_CASE(Clear)
	{
		constexpr int controlSize = 10;

		auto testObject = my::List<TestType, std::allocator<TestType>>();
		for (int i = 1; i <= controlSize; ++i)
		{
			testObject.insert(testObject.end(), TestType{ i,i });
		}

		testObject.clear();

		BOOST_TEST_CHECK(testObject.empty());
	}

	BOOST_AUTO_TEST_CASE(Remove)
	{
		constexpr int controlSize = 10;
		TestType controlValue{ 3,3 };

		auto testObject = my::List<TestType, std::allocator<TestType>>();
		for (int i = 1; i <= controlSize; ++i)
		{
			testObject.insert(testObject.end(), TestType{ i,i });
		}

		testObject.remove(controlValue);

		for (auto elem : testObject)
		{
			BOOST_CHECK(elem != controlValue);
		}

		BOOST_CHECK(testObject.size() == controlSize - 1);
	}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

	BOOST_AUTO_TEST_SUITE(TestMyReserveAllocator)

	BOOST_AUTO_TEST_CASE(TestAddItemOverMaxSizeInMyList)
	{
		my::List<Hard, my::ReserveAllocator<Hard, 200>> testList;

		for(int i = 0;i < testList.max_size();++i)
		{
			testList.push_back(Hard{ i,i });
		}

		BOOST_CHECK_THROW(testList.push_back(Hard{ 1,1 }),std::runtime_error);
	}

	BOOST_AUTO_TEST_CASE(TestAddItemOverMaxSizeInStdList)
{
	std::list<Hard, my::ReserveAllocator<Hard, 200>> testList;


	BOOST_TEST_MESSAGE(testList.max_size());

	for (int i = 0; i < testList.max_size() - 1 ; ++i)
	{
		testList.push_back(Hard{ i,i });
	}

	BOOST_CHECK_THROW(testList.push_back(Hard{ 1,1 }), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()