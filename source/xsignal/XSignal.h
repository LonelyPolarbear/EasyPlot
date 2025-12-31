#pragma  once

#include <memory>
#include <optional>
#include <boost/signals2.hpp>

namespace xsig {
	struct NoMutex {
		void lock() {}
		void unlock() {}
		bool try_lock() { return true; }
	};

	template<typename T>
	struct traits_class {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::shared_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::unique_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	struct traits_class<std::weak_ptr<T>> {
		using classType = T;
	};

	template<typename T>
	using traits_class_t = typename traits_class<T>::classType;

	template<typename T>
	struct remove_cvref {
		using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
	};

	template<typename T>
	using remove_cvref_t = typename remove_cvref<T>::type;

	template <typename T>
	static T* extract_origin_pointer(T* p)
	{
		return p;
	}

	template <typename T>
	static T* extract_origin_pointer(std::weak_ptr<T> p)
	{
		auto sharePtr = p.lock();
		T* other = nullptr;
		if (sharePtr)
		{
			other = sharePtr.get();
		}
		return other;
	}

	template <typename T>
	static T* extract_origin_pointer(std::shared_ptr<T> p)
	{
		T* other = nullptr;
		if (p)
		{
			other = p.get();
		}
		return other;
	}

	template<typename T>
	class xoptional_last_value
	{
	public:
		using result_type = std::optional<T>;

		template<typename InputIterator>
		std::optional<T> operator()(InputIterator first, InputIterator last) const
		{
			std::optional<T> value;
			while (first != last)
			{
				{
					try {
						value = *first;
					}
					catch (const boost::signals2::expired_slot&) {}
				}
				++first;
			}
			return value;
		}
	};

	template<>
	class xoptional_last_value<void>
	{
	public:
		using result_type = void;

		template<typename InputIterator>
		void operator()(InputIterator first, InputIterator last) const
		{
			while (first != last)
			{
				{
					try {
						*first;
					}
					catch (const boost::signals2::expired_slot&) {}
				}
				++first;
			}
			return;
		}
	};

	template<typename T>
	class xoptional_all_value
	{
	public:
		using result_type = std::vector< std::optional<T>>;

		template<typename InputIterator>
		result_type operator()(InputIterator first, InputIterator last) const
		{
			result_type values;
			while (first != last)
			{
				{
					try {
						values.push_back(*first);
					}
					catch (const boost::signals2::expired_slot&) {}
				}
				++first;
			}
			return values;
		}
	};

	template<>
	class xoptional_all_value<void>
	{
	public:
		using result_type = void;

		template<typename InputIterator>
		void operator()(InputIterator first, InputIterator last) const
		{
			while (first != last)
			{
				{
					try {
						*first;
					}
					catch (const boost::signals2::expired_slot&) {}
				}
				++first;
			}
			return;
		}
	};

	//template<typename Signature>
	//using xsignal_wrapper = boost::signals2::signal<
	//	Signature,
	//	xoptional_all_value<typename boost::function_traits<Signature>::result_type>,
	//	int,												//用于对连接进行分组的类型，以便按特定顺序调用或管理
	//	std::less<int>,								//用于确定分组调用的顺序。槽函数按照分组值的升序调用（默认）
	//	std::function<Signature>,
	//	typename boost::signals2::detail::extended_signature<
	//	boost::function_traits<Signature>::arity,
	//	Signature
	//	>::function_type,
	//	boost::signals2::mutex	//boost::signals2::dummy_mutex  // 非线程安全，但更快
	//>;


	struct xconnection {
		xconnection()=default;

		explicit xconnection(boost::signals2::connection con) {
			handle = con;
		}
		
		void disconnect() {
			handle.disconnect();
		}
		boost::signals2::connection handle;

		bool connected() const {
			return handle.connected();
		}

		explicit operator bool() const {
			return connected();
		}

		 operator boost::signals2::connection() const {
			return handle;
		}
	};

	template<typename F>
	class xsignal;

	template<typename R, typename... Args>
	struct xsignal<R(Args...)> {
		using bsignal = boost::signals2::signal<R(Args...)/*, xoptional_all_value<R>*/>;
		bsignal signal;

		template<typename ...types>
		decltype(auto) operator()(types&&... args) const {
			 return signal(std::forward<types>(args)...);
		}

		template<typename ...types>
		decltype(auto) xemit(types&&... args) const {
			return operator()(std::forward<types>(args)...);
		}


		//连接成员函数
		template<class desttype,typename ...types>
		xconnection connect(desttype pclass, R(traits_class_t<std::remove_pointer_t<desttype>>::* f)(types...))
		{
			auto origin_ptr = extract_origin_pointer(pclass);
			if (origin_ptr) {
				auto fun = [pclass, f](Args... args)->R {
					auto origin_ptr = extract_origin_pointer(pclass);
					if (origin_ptr)
						return (origin_ptr->*f)(args...);
					};
				xconnection connection;
				connection.handle = signal.connect(fun);
				return connection;
			}
			else {
				return {};
			}
		}

		//连接信号
		template<class desttype>
		xconnection connect(desttype pclass, xsignal<R(Args...)> traits_class_t<std::remove_pointer_t<desttype>>::* signalObj) 
		{
			auto origin_ptr = extract_origin_pointer(pclass);
			if (origin_ptr) {
				auto fun = [pclass, signalObj](Args... args)->R {
					auto origin_ptr = extract_origin_pointer(pclass);
					if (origin_ptr)
						return *(origin_ptr->*signalObj)(args...);
					};
				xconnection connection;
				connection.handle = signal.connect(fun);
				return connection;
			}
			else {
				return {};
			}
		}
		
		//连接全局函数和仿函数
		template<typename F>
		xconnection connect(F&& f)
		{
			xconnection connection;
			connection.handle = signal.connect(f);
			return connection;
		}

		// 获取连接数量
		std::size_t connection_count() const {
			return signal.num_slots();
		}
	};

	template<typename T>
	struct is_xsignal : std::false_type {};

	template<typename R, typename... Args>
	struct is_xsignal<xsignal<R(Args...)>> : std::true_type {
		using function_type = R(Args...);
		static constexpr bool isBoostSig = false;
	};

	template<typename R, typename... Args>
	struct is_xsignal<boost::signals2::signal<R(Args...)>> : std::true_type {
		using function_type = R(Args...);
		static constexpr bool isBoostSig = true;
	};

	template<typename classType,typename FunType>
	struct trait_member_fun_type;

	template<typename classType, typename R,typename ...Args>
	struct trait_member_fun_type<classType, R(Args...)> {
		using type = R(classType::*)(Args...);
	};


	template<typename T>
	struct boost_sig_connect;

	template<typename R, typename...Args>
	struct boost_sig_connect<R(Args...)> {
		//连接成员函数
		template<typename signaltype, typename recivertype>
		static decltype(auto) connect(signaltype* signalObj, recivertype pclass, R(traits_class_t<std::remove_pointer_t<recivertype>>::* f)(Args...)) {
			auto origin_ptr = extract_origin_pointer(pclass);
			if (origin_ptr) {
				auto fun = [pclass, f](Args... args)->R {
					auto origin_ptr = extract_origin_pointer(pclass);
					if (origin_ptr)
						return (origin_ptr->*f)(args...);
					};
				return signalObj->connect(fun);
			}
		}

		//连接信号
		template<typename signaltype, typename recivertype>
		static decltype(auto) connect(signaltype* signalObj, recivertype pclass, xsignal<R(Args...)> traits_class_t<std::remove_pointer_t<recivertype>>::* signalObj2) {
			auto origin_ptr = extract_origin_pointer(pclass);
			if (origin_ptr) {
				auto fun = [pclass, signalObj2](Args... args)->R {
					auto origin_ptr = extract_origin_pointer(pclass);
					if (origin_ptr)
						return *(origin_ptr->*signalObj2)(args...);
					};
				return signalObj->connect(fun);
			}
		}

		//连接仿函数和全局函数
		template<typename signaltype, typename slottype>
		static decltype(auto) connect(signaltype* signalObj, slottype f) {
			return signalObj->connect(f);
		}
	};

	template<typename signaltype,
		typename recivertype,
		typename reciverClass = traits_class_t<     xsig::remove_cvref_t< std::remove_pointer_t<recivertype>   >   >,
		typename slottype /*= typename  trait_member_fun_type<  reciverClass, typename is_xsignal<signaltype>::function_type  >::type*/,
		typename T = std::enable_if_t <
		std::is_class_v<reciverClass>&&
		(std::is_member_function_pointer_v<slottype> || std::is_member_object_pointer_v<slottype>)&&
		is_xsignal<signaltype>::value&&
		is_xsignal<signaltype>::isBoostSig
		>
	>
	 decltype(auto) bSigConnect(signaltype* signalObj, recivertype pclass, slottype f) {
		 return boost_sig_connect<typename is_xsignal<signaltype>::function_type>::connect(signalObj, pclass, f);
	}

	//连接仿函数和全局函数
	 template<typename signaltype,
		 typename slottype = std::function< typename is_xsignal<signaltype>::function_type>,
		 typename T = std::enable_if_t <
		 is_xsignal<signaltype>::value&&
		 is_xsignal<signaltype>::isBoostSig
	 > >
	 static decltype(auto) bSigConnect(signaltype* signalObj, slottype f) {
		 return boost_sig_connect<typename is_xsignal<signaltype>::function_type>::connect(signalObj, f);
	 }


	template <typename sendertype, typename signaltype, typename recivertype,
		typename senderClass = traits_class_t<     xsig::remove_cvref_t< std::remove_pointer_t<sendertype>   >   >,
		typename reciverClass = traits_class_t<     xsig::remove_cvref_t< std::remove_pointer_t<recivertype>   >   >,
		typename slottype /*= typename  trait_member_fun_type<  reciverClass, typename is_xsignal<signaltype>::function_type  >::type*/,
		typename T = std::enable_if_t<
		std::is_class_v<senderClass>&&
		std::is_class_v<reciverClass>&&
		(std::is_member_function_pointer_v<slottype>  || std::is_member_object_pointer_v<slottype>) &&
		is_xsignal<signaltype>::value
		>
	>
	xconnection connect(sendertype sender, signaltype senderClass::* signalObj, recivertype recv, slottype f)
	{
		auto origin_ptr = extract_origin_pointer(sender);
		if (origin_ptr) {
			if constexpr (is_xsignal<signaltype>::isBoostSig) {
				auto con = bSigConnect(&(origin_ptr->*signalObj), recv, f);
				xconnection ret;
				ret.handle = con;
				return ret;
			}
			else {
				return (origin_ptr->*signalObj).connect(recv, f);
			}
		}	
		return {};
	}

	template <typename sendertype, typename signaltype,
		typename senderClass = traits_class_t<     xsig::remove_cvref_t< std::remove_pointer_t<sendertype>   >   >,
		typename slottype =std::function< typename is_xsignal<signaltype>::function_type> ,
		typename T = std::enable_if_t<
			std::is_class_v<senderClass>&&
			is_xsignal<signaltype>::value
		>
	>
	xconnection connect(sendertype sender, signaltype senderClass::* signalObj, slottype f)
	{
		auto origin_ptr = extract_origin_pointer(sender);
		if (origin_ptr) {
			if constexpr (!is_xsignal<signaltype>::isBoostSig)
				return (origin_ptr->*signalObj).connect(f);
			else {
				auto con =  bSigConnect(&(origin_ptr->*signalObj),f);
				xconnection ret;
				ret.handle = con;
				return ret;
			}
		}		
		return {};
	}
}

#define XSIGNAL(...) xsig::xsignal<__VA_ARGS__>