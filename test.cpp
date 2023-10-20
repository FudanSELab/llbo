template <typename T>
concept Instrumentable = requires(T t) {
  {t.instrument()};
};

int main() {}
