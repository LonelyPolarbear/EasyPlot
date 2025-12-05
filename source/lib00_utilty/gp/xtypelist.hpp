#ifndef XTYPELSIT_HPP
#define XTYPELSIT_HPP
#include <type_traits>
/**
* @file         xtypelist.hpp
* @brief       typelist模板类，用于泛型编程
* @author     宋伟军
* @date        2025.4.22
* @license Copyright(c)2023, 英特工程仿真技术(大连)有限公司版权所有
*/
namespace TL
{   
    //typelist 定义
    template <typename...>
    struct typelsit{};

    //base op1--获取头节点
    template<typename TList>
    struct TL_Front;

    template<typename head,typename... tails>
    struct TL_Front<typelsit<head,tails...>>{
        using type = head;
    };

    template<typename TList>
    using TL_Front_t = typename TL_Front<TList>::type;

    //base op2--删除头节点
    template<typename TList>
    struct TL_PopFront;

    template<typename head,typename... tails>
    struct TL_PopFront<typelsit<head,tails...>>{
        using type = typelsit<tails...>;
    };

    template<typename TList>
    using  TL_PopFront_t = typename TL_PopFront<TList>::type;

    //base op3--从头节点插入一个元素
    template<typename TList,typename T>
    struct TL_PushFront;

    template<typename T,typename... tails>
    struct TL_PushFront<typelsit<tails...>,T>{
        using type = typelsit<T,tails...>;
    };

    template<typename TList,typename T>
    using TL_PushFront_t = typename TL_PushFront<TList,T>::type;

    template<typename Tlist>
    struct TL_sizeof;

    template<typename ...tails>
    struct TL_sizeof<typelsit<tails...>>{
        static constexpr int value = sizeof...(tails);
    };

    template<typename Tlist>
    static constexpr int TL_sizeof_v = TL_sizeof<Tlist>::value;

    template<unsigned int first,unsigned int second>
    static constexpr bool TL_isEqual_v = (first == second);
}

namespace TL
{
    template<typename TList,typename T,unsigned N,bool = TL_isEqual_v<TL_sizeof_v<TList>, 1> >
    struct TL_indexOf;

    template<typename TList,typename T,unsigned N>
    struct TL_indexOf<TList,T,N,false>{
        static constexpr int val = std::is_same_v<TL_Front_t<TList>,T> ? N: TL_indexOf<TL_PopFront_t<TList>,T,N+1>::val;
    };

    template<typename TList,typename T,unsigned N>
    struct TL_indexOf<TList,T,N,true>{
        static constexpr int val = std::is_same_v<TL_Front_t<TList>,T> ? N:N+1;
    };

    template<typename TList,typename T>
    constexpr int TL_indexOf_v = TL_indexOf<TList,T,0>::val;

    template<typename TList,unsigned int index>
    struct TL_NthElement{
        using type = typename TL_NthElement<TL_PopFront_t<TList>,index-1>::type;
    };

    template<typename Tlist>
    struct TL_NthElement<Tlist,0>{
        using type = TL_Front_t<Tlist>;
    };

    template<typename TList,unsigned int index>
    using TL_NthElement_t = typename TL_NthElement<TList,index>::type;

    template<bool flag,typename first,typename second>
    struct ifThenElse{
        using type = first;
    };

    template<typename first,typename second>
    struct ifThenElse<false,first,second>{
        using type = second;
    };

    template<bool flag,typename first,typename second>
    using ifThenElse_t = typename ifThenElse<flag,first,second>::type;

    template<typename first,typename second>
    struct sizeCompare{
       static constexpr bool value = sizeof(first) >= sizeof(second);
    };
    
    template<typename TList,template<typename,typename> class compare,bool = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_findBestMatch;

    template<typename TList,template<typename,typename> class compare>
    struct TL_findBestMatch<TList,compare,false>{
    private:
        using Contender =  TL_Front_t<TList>;
        using rest = typename TL_findBestMatch<TL_PopFront_t<TList>,compare>::type;
   
    public:
        using type = ifThenElse_t<compare<Contender,rest>::value,Contender,rest>;
    };

    template<typename TList,template<typename,typename> class compare>
    struct TL_findBestMatch<TList,compare,true>{
    public:
        using type = TL_PopFront_t<TList>;
    };

    template<typename TList,template<typename,typename> class compare>
    using TL_findBestMatch_t = typename TL_findBestMatch<TList,compare>::type;

    template<typename TList,typename element,bool  = TL_isEqual_v<TL_sizeof_v<TList>, 0>>
    struct TL_pushback;

    template<typename TList,typename element>
    struct TL_pushback<TList,element,true>{
        using type =typelsit<element>;
    };

    template<typename TList,typename element>
    struct TL_pushback<TList,element,false>{
    private:
        using head = TL_Front_t<TList>;
        using tail = TL_PopFront_t<TList>;

        using newTail = typename TL_pushback<tail,element>::type;
    public:
        using type = TL_PushFront_t<newTail,head>;
    };

    template<typename TList,typename element>
    using TL_pushback_t = typename TL_pushback<TList,element>::type;

    template<typename TList,bool  = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_reverse;

    template<typename TList>
    struct TL_reverse<TList,false>{
        using head = TL_Front_t<TList>;
        using tail = TL_PopFront_t<TList>;
    public:
        using type = TL_pushback_t<typename TL_reverse<tail>::type,head>;
    };

    template<typename TList>
    struct TL_reverse<TList,true>{
        using type = typelsit<TL_Front_t<TList>>;
    };

    template<typename TList>
    using TL_reverse_t =typename TL_reverse<TList>::type;

    template<typename TList>
    struct TL_popBack{
    private:
        using reverse = TL_reverse_t<TList>;
        using tmp = TL_PopFront_t<reverse>;
    public:
        using type = TL_reverse_t<tmp>;
    };

    template<typename TList>
    using TL_popBack_t = typename TL_popBack<TList>::type;

    template<typename TList,template<typename> class Fun,bool  = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_transform;

    template<typename TList,template<typename> class Fun>
    struct TL_transform<TList,Fun,false>{
    private:
        using head = TL_Front_t<TList>;
        using tail = typename TL_transform<TL_PopFront_t<TList>,Fun>::type;
        using Trans_head =typename Fun<head>::type ;
    public:
        using type = TL_PushFront_t<tail,Trans_head>;
    };

    template<typename TList,template<typename> class Fun>
    struct TL_transform<TList,Fun,true>{
    private:
        using head = TL_Front_t<TList>;
        using Trans_head =typename Fun<head>::type ;
    public:
        using type = typelsit<Trans_head>;
    };


    /*
    * 新加判断是否所有类型都符合某个条件
    */

	template<typename TList, template<typename> class Fun, bool = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_is;

    template<typename TList, template<typename> class Fun>
    struct TL_is<TList,Fun,true>{
        using head = TL_Front_t<TList>;
        static constexpr  bool value = Fun<head>::value;
    };

	template<typename TList, template<typename> class Fun>
	struct TL_is<TList, Fun, false> {
	private:
		using head = TL_Front_t<TList>;
		using tail =  TL_PopFront_t<TList>;
	public:
        static constexpr  bool value = Fun<head>::value && TL_is<tail,Fun>::value;
	};

    template<typename TList,typename element,template<typename,typename> class compare,bool  = TL_isEqual_v<TL_sizeof_v<TList>, 0>>
    struct TL_insertSoreted;

    template<typename TList,typename element,template<typename,typename> class compare>
    struct TL_insertSoreted<TList,element,compare,false>{
    public:
        using newTail = ifThenElse_t< compare<element,TL_Front_t<TList>>::value,
                                                        TList,
                                                        typename TL_insertSoreted<TL_PopFront_t<TList>,element,compare>::type
                                                        >;
        using newHead = ifThenElse_t< compare<element,TL_Front_t<TList>>::value,
                                                        element,
                                                        TL_Front_t<TList>
                                                        >;
    public:
        using type = TL_PushFront_t<newTail,newHead>;
    };
    
    template<typename TList,typename element,template<typename,typename> class compare>
    struct TL_insertSoreted<TList,element,compare,true>{
        using type = typelsit<element>;
    };

    template<typename first,typename second>
    struct compareSize{
        static constexpr bool value = (sizeof(first)<sizeof(second)); 
    };

    template<typename TList,template<typename,typename> class compare,bool  = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_sort;

    template<typename TList,template<typename,typename> class compare>
    struct TL_sort<TList,compare,false>{
        using head = TL_Front_t<TList>;
        using tail = TL_PopFront_t<TList>;

        using newTail = typename TL_sort<tail,compare>::type;

        using type =typename TL_insertSoreted<newTail,head,compare>::type;
    };

    template<typename TList,template<typename,typename> class compare>
    struct TL_sort<TList,compare,true>{
        using type = typelsit<TL_Front_t<TList>>;
    };

    //查找某个类最近的基类
    //TList是按照子类在前，基类在后的顺序排列
    template<typename TList,typename T, template<typename, typename> class compare,bool = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_FindBase{
        using type = TL_Front_t<TList>;
    };

    template<typename TList, typename T, template<typename, typename> class compare>
    struct TL_FindBase<TList,T,compare,false>{
        using Contender = TL_Front_t<TList>;
        using rest = typename TL_FindBase<TL_PopFront_t<TList>,T, compare>::type;

    public:
        using type = ifThenElse_t<compare<Contender, T>::value && !std::is_same_v<Contender,T>, Contender, rest>;
    };

    template<typename TList, typename T>
    using TL_FindBase_t = typename TL_FindBase<TList, T,std::is_base_of>::type;

    //新增接口，判断typelist是否存在某个元素
    template<typename TList, typename E>
    constexpr bool TL_exits_v = TL_indexOf_v<TList, E> != TL_sizeof_v<TList>;

    //去重
    template<typename TList, bool = TL_isEqual_v<TL_sizeof_v<TList>, 1>>
    struct TL_deduplicate;

    template<typename TList>
    struct TL_deduplicate<TList, false> {
        using head = TL_Front_t<TList>;
        using tail = TL_PopFront_t<TList>;
    public:
        using type = ifThenElse_t <
            TL_exits_v<tail, head>,
            typename TL_deduplicate<tail>::type,
            TL_PushFront_t<typename TL_deduplicate<tail>::type, head>
        >;
    };

    template<typename TList>
    struct TL_deduplicate<TList, true> {
        using type = typelsit<TL_Front_t<TList>>;
    };

    template<typename TList>
    using TL_deduplicate_t = typename TL_deduplicate<TList>::type;


    //关于双重派发的实现
    template<typename TList>
    using sortByInheritence_base =typename TL_sort<TList,std::is_base_of>::type;

    template<typename TList>
    using sortByInheritence_child =TL_reverse_t<sortByInheritence_base<TList>>;

    //单通道派发实现
    template <class Executor, class Base, class TList>
    struct singleDispatcherImpl {
        using head = TL_Front_t<TList>;
        using tail = TL_PopFront_t<TList>;
    public:
        template<typename ...Args>
        static decltype(auto) Dispatch(Base* pleft,  Args&&... args)
        {
            if (head* p = dynamic_cast<head*>(pleft))
            {
                return Executor::execute( p, std::forward<Args>(args)...);
            }
            else
            {
                if constexpr (TL_sizeof_v<tail> >=1) {
                    return singleDispatcherImpl<Executor, Base, tail>::Dispatch(pleft, std::forward<Args>(args)...);
                }
                    
            }
        }
    };

    //单通道派发接口
    template <class Executor, class Base, class TList>
    struct singleDispatcher {
        using types = sortByInheritence_child<TL_deduplicate_t<TList>>;
        template<typename ...Args>
        static decltype(auto) Dispatch(Base* left,Args&&... args)
        {
            return singleDispatcherImpl<Executor, Base, types>::Dispatch(left,std::forward<Args>(args)...);
        }
    };

    template<typename TList, class Executor>
    struct superClassCall {
        using types = sortByInheritence_child<TL_deduplicate_t<TList>>;
        template<typename PtrClass,typename ...Args>
        static decltype(auto) execute(PtrClass* left, Args&&... args)
        {
            using superClass =  TL_FindBase_t<types, PtrClass>;
            return Executor::execute((superClass*)left, std::forward<Args>(args)...);
        }
    };

    template <class Executor, class BaseRhs, class left_find, class TList_R,bool  = TL_isEqual_v<TL_sizeof_v<TList_R>, 0>>
    struct StaticDispatcherRhs;

    template <class Executor, class BaseRhs, class left_find, class TList_R>
    struct StaticDispatcherRhs<Executor,BaseRhs,left_find,TList_R,false>
    {
        using right_head = TL_Front_t<TList_R>;
        using right_tail = TL_PopFront_t<TList_R>;
    public:
        static void Dispatch(left_find *pleft, BaseRhs *pright)
        {
            if (right_head *p = dynamic_cast<right_head *>(pright))
            {
                return Executor::execute(pleft, p);
            }
            else
            {
                return StaticDispatcherRhs<Executor, BaseRhs, left_find, right_tail>::Dispatch(pleft, pright);
            }
        }
    };

    template <class Executor, class BaseRhs, class left_find, class TList_R>
    struct StaticDispatcherRhs<Executor,BaseRhs,left_find,TList_R,true>{
        static void Dispatch(left_find *pleft, BaseRhs *pright)
        {
            return Executor::execute(pleft,pright);
        }
    };

    template <class Executor, class BaseLhs, class BaseRhs, class TList_L, class TList_R,bool  = TL_isEqual_v<TL_sizeof_v<TList_L>, 0>>
    struct StaticDispatcherLhs;

    template <class Executor, class BaseLhs, class BaseRhs, class TList_L, class TList_R>
    struct StaticDispatcherLhs<Executor,BaseLhs,BaseRhs,TList_L,TList_R,false>
    {
        using left_head = TL_Front_t<TList_L>;
        using left_tail = TL_PopFront_t<TList_L>;

    public:
        static void Dispatch(BaseLhs *pleft, BaseRhs *pright)
        {
            if (left_head *p = dynamic_cast<left_head *>(pleft))
            {
                return StaticDispatcherRhs<Executor, BaseRhs, left_head, TList_R>::Dispatch(p, pright);
            }
            else
            {
                return StaticDispatcherLhs<Executor, BaseLhs, BaseRhs, left_tail, TList_R>::Dispatch(pleft, pright);
            }
        }
    };

    //特化处理
    template <class Executor, class BaseLhs, class BaseRhs, class TList_L, class TList_R>
    struct StaticDispatcherLhs<Executor, BaseLhs, BaseRhs, TList_L, TList_R, true>
    {
    public:
        static void Dispatch(BaseLhs *pleft, BaseRhs *pright)
        {
            return StaticDispatcherRhs<Executor, BaseRhs, BaseLhs, TList_R>::Dispatch(pleft, pright);
        }
    };

    //接口
    template <class Executor, class BaseLhs, class TList_L, class BaseRhs = BaseLhs, class TList_R = TList_L>
    struct doubleDispatcher
    {
        using typesLh = sortByInheritence_child<TList_L>;
        using typesRh = sortByInheritence_child<TList_R>;
        static void Dispatch(BaseLhs *pleft, BaseRhs *pright)
        {
            return StaticDispatcherLhs<Executor, BaseLhs, BaseRhs,typesLh, typesRh>::Dispatch(pleft, pright);
        }
    };
}
#endif