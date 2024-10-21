#include <string_view>
#include <variant>
#include <iostream>

void print_arg(std::string_view arg) {
    std::cout << arg << '\n';
}

template <typename V, size_t I = 0, typename ... Args>
constexpr auto get(std::string_view type, auto func, Args... args) {
    if constexpr (I < std::variant_size_v<V>) {
        using Candidate = std::variant_alternative_t<I, V>;
        if (Candidate::type() == type) {
            Candidate::process(func, std::forward<Args>(args)...);
        } else {            
            get<V, I + 1>(type, func, std::forward<Args>(args)...);
        }
    } else {
        throw std::invalid_argument("Error [get]: Invalid type");  // Error
    }
}

template <typename ... Args>
void start_chain(auto func, std::string_view type, Args... args) {
    func(type, std::forward<Args>(args)...);
}

template <typename T>
void print() {
    std::cout << T::type() << std::endl;
}


class C2 {
public:
    static constexpr auto type() -> std::string_view {
        return "C2";
    }      
    
    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising B2...\n";
        next_func(std::forward<Args>(args)..., C2{});
    }
};

class C1 {
public:
    static constexpr auto type() -> std::string_view {
        return "C1";
    }     
    
    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising C1...\n";
        next_func(std::forward<Args>(args)..., C1{});
    } 
};

class B2 {
public:
    static constexpr auto type() -> std::string_view {
        return "B2";
    }      
    
    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising C2...\n";
        next_func(std::forward<Args>(args)..., B2{});
    }
};

class B1 {
public:
    static constexpr auto type() -> std::string_view {
        return "B1";
    }     
    
    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising B1...\n";
        next_func(std::forward<Args>(args)..., B1{});
    } 
};


class A2 {
public:
    static constexpr auto type() -> std::string_view {
        return "A2";
    }    
       

    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising A2...\n";
        next_func(std::forward<Args>(args)..., A2{});
    }
};

class A1 {
public:
    static constexpr auto type() -> std::string_view {
        return "A1";
    }    

    template <typename... Args>
    static void process(auto next_func, Args... args) {
        std::cout << "Initialising A1...\n";
        next_func(std::forward<Args>(args)..., A1{});
    }
};

int main() {
    using v_A = std::variant<A1, A2>;
    using v_B = std::variant<B1, B2>;
    using v_C = std::variant<C1, C2>;

    auto get_a = []<typename... Args>(std::string_view type, Args... args){get<v_A>(type, std::forward<Args>(args)...);};
    auto get_b = []<typename... Args>(std::string_view type, Args... args){get<v_B>(type, std::forward<Args>(args)...);};
    auto get_c = []<typename... Args>(std::string_view type, Args... args){get<v_C>(type, std::forward<Args>(args)...);};
    
    auto print_processors = []<typename T1, typename T2, typename T3>(T1 t1, T2 t2, T3 t3) {
        std::cout << "Processing:\n";
        std::cout << t1.type() << "\n";
        std::cout << t2.type() << "\n";
        std::cout << t3.type() << "\n";
    };

    start_chain(get_a, "A1", get_b, "B1", get_c, "C2", print_processors);

    std::cout << "Program completed successfully" << '\n';
}