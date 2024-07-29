#include <iostream>
#include <tuple>

#include <gtest/gtest.h>
// set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest)
// set(FETCHCONTENT_QUIT OFF)

// include(FetchContent)
// FetchContent_Declare(
//     googletest
//     URL https://github.com/google/googletest/releases/download/v1.15.0/googletest-1.15.0.tar.gz
//     URL_HASH MD5=1fb60c818501bcecee87349cf2e7377f
//     # SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/googletest
// )

// if (WIN32)
//     set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
// endif()
// FetchContent_MakeAvailable(googletest)

// include(fetchgoogletest)
// enable_testing()
// target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest_main)
// include(GoogleTest)
// gtest_discover_tests(${PROJECT_NAME})

// ::testing::InitGoogleTest(&argc, argv);
// return RUN_ALL_TESTS();

namespace move_assigned_if_noexcption
{
    class moveAssignedAble
    {
        public:
            moveAssignedAble& operator=(const moveAssignedAble&){std::cout<<"copy assign"<<std::endl;return *this;}
            moveAssignedAble& operator=(moveAssignedAble&&){std::cout<<"move assign"<<std::endl;return *this;}
    };

    class moveAssignedAbleNoexception
    {
        public:
            moveAssignedAbleNoexception& operator=(const moveAssignedAbleNoexception&){std::cout<<"copy assign"<<std::endl;return *this;}
            moveAssignedAbleNoexception& operator=(moveAssignedAbleNoexception&&) noexcept {std::cout<<"move assign noexcept"<<std::endl;return *this;}
    };

    template<typename  T>
    constexpr std::conditional<std::is_nothrow_move_assignable<T>::value, T&&, const T&>::type move_assigned_if_noexcption(const T& v){return std::move(v);}

    class fixture : public ::testing::Test
    {
        protected:
            fixture(){};
            virtual ~fixture() override {}

            virtual void SetUp() override{}
            virtual void TearDown() override{}

            static std::size_t getcount() {static std::size_t count{0}; return ++count;}

            std::string name{"hello world"};
    };

    TEST(moveobjecttest, test1)
    {
        moveAssignedAble a;
        EXPECT_EQ(std::is_lvalue_reference<decltype(move_assigned_if_noexcption(a))>::value, true) << "moveAssignedAble move assign operator is noexcept!, u need remove noexcept";
    }

    TEST(moveobjecttest, test2)
    {
        moveAssignedAbleNoexception b;
        EXPECT_EQ(std::is_rvalue_reference<decltype(move_assigned_if_noexcption(b))>::value, true) << "moveAssignedAbleNoexception move assign operator is not noexcept!, u need addtion noexcept";
    }


    TEST_F(fixture, test1)
    {
        EXPECT_EQ((name == "hello world"), true) << "fixture.name != hello world";
        name = "testing";
        EXPECT_EQ(name == "testing", true) << "name no changed to testing";
        EXPECT_EQ(getcount() == 1, true) << "getcount() return != 1";
    }

    TEST_F(fixture, test2)
    {
        ASSERT_EQ(name == "hello world", true) << "fixture not be init before new TEST_F call";
        EXPECT_EQ(getcount() == 1, true) << "getcount() return != 1"; //fixture static is not common in gtest
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}