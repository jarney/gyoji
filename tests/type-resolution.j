
class Inner;

class Outer {
    Inner *outer_ptr;
    i32 int_value;
    u8 char_value;
};

class Inner {
    public u32 unsigned_value;
    public f32 float_value;
};

namespace epsilon {
    class SmallValue {
        f64 small_epsilon_value;
    };
};

namespace JLang {
    using namespace epsilon as phi;
    
    class Namespace;
    class NamespaceContext;

    class Namespace {
        public u8 *name;
    };

    class NamespaceContext {
        public Namespace *ns_map;
        public Inner *other_context;
        public phi::SmallValue val;
    };
    
};
