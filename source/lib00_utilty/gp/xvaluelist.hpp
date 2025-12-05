#ifndef XVALUELSIT_HPP
#define XVALUELSIT_HPP

/**
* @file         xvaluelist.hpp
* @brief       valuelist模板类，用于泛型编程
* @author     宋伟军
* @date        2025.4.22
* @license Copyright(c)2023, 英特工程仿真技术(大连)有限公司版权所有
*/

namespace VL
{
    template<typename T,T...>
    struct valuelist{
        using type = T;
    };

    //base op1--获取头节点
    template<typename VList>
    struct VL_Front;\

    template<typename T,T head,T... tails>
    struct VL_Front<valuelist<T,head,tails...>>{
        static constexpr T value = head;
    };

    template<typename VList>
    constexpr typename VList::type VL_Front_v = VL_Front<VList>::value;

    //base op2--删除头节点
    template<typename TList>
    struct VL_PopFront;

    template<typename T, T head,T... tails>
    struct VL_PopFront<valuelist<T,head,tails...>>{
        using type = valuelist<T,tails...>;
    };

    template<typename VList>
    using VL_PopFront_t = typename VL_PopFront<VList>::type;

    //base op3--从头节点插入一个元素
    template<typename TList,typename TList::type N>
    struct VL_PushFront;

    template<typename T,T N,T... Args>
    struct VL_PushFront<valuelist<T,Args...>,N>{
        using type = valuelist<T,N,Args...>;
    };

    template<typename TList,typename TList::type N>
    using VL_PushFront_t = typename VL_PushFront<TList,N>::type;

    template<typename Tlist>
    struct VL_sizeof;

    template<typename T,T...tails>
    struct VL_sizeof<valuelist<T,tails...>>{
        static constexpr int value = sizeof...(tails);
    };

    template<typename Vlist>
    constexpr int VL_sizeof_v = VL_sizeof<Vlist>::value;
} // namespace VL

namespace VL
{   
    template<typename VList,typename VList::type element>
    struct VL_pushBack;

    template<typename T,T element,T... Args>
    struct VL_pushBack<valuelist<T,Args...>,element>{
        using type = valuelist<T,Args...,element>;
    };

    template<typename VList,typename VList::type element>
    using VL_pushBack_t =typename VL_pushBack<VList,element>::type;

    template<unsigned int N>
    struct xmake_IndexSquence{
        using lastType =typename xmake_IndexSquence<N-1>::type;
        using type = VL_pushBack_t<lastType,N>;
    };

    template<>
    struct xmake_IndexSquence<0>{
        using type = valuelist<unsigned int,0>;
    };

     template<unsigned int N>
    using xmake_IndexSquence_t = typename xmake_IndexSquence<N>::type;
} // namespace VL
#endif