/* The following header is part of CPP-Util version 334d.
 * The upstream can be found at: https://github.com/agvxov/cpp-util
 * It is in the Public Domain.
 */
/* Resource Manager
 *
 * Maps arbitrary resources to strings.
 * If a resource has yet to be loaded, it will attempt to load it.
 * When loading fails, a default resource is returned.
 *
 * Guarantees that the dev will get back a valid resource.
 *
 * For example it could be used to seemlessly load/fetch resources in a game.
 */
#include <unordered_map>
#include <stdexcept>
#include <string>

template<typename T>
class Resource_manager {
    T (*load_resource)(const char * s);
    bool (*is_resource_valid)(T r);
    void (*unload_resource)(T r);

    std::unordered_map<std::string, T> resources;
    T default_resource;

  public:
    Resource_manager(
      T (*load_resource_)(const char * s),
      void (*unload_resource_)(T r),
      bool (*is_resource_valid_)(T r),
      T default_resource_
    )
      : load_resource(load_resource_)
      , is_resource_valid(is_resource_valid_)
      , unload_resource(unload_resource_)
      , default_resource(default_resource_)
    {
        ;
    }

    Resource_manager(
      T (*load_resource_)(const char * s),
      void (*unload_resource_)(T r),
      bool (*is_resource_valid_)(T r),
      const char * default_key
    )
      : load_resource(load_resource_)
      , is_resource_valid(is_resource_valid_)
      , unload_resource(unload_resource_)
    {
        T default_resource_ = load_resource_(default_key);

        if (!is_resource_valid(default_resource_)) {
            throw std::runtime_error(
              std::string("failed to load default resource: ") + default_key
            );
        }

        resources[default_key] = default_resource_;
    }

    ~Resource_manager() {
        for (auto [k, v] : resources) {
            unload_resource(v);
        }
    }

    T load(const char * id) {
        T r = load_resource(id);
        if (!is_resource_valid(r)) {
            r = default_resource;
        }
        resources[id] = r;

        return r;
    }

    T get(const char * id) {
        auto it = resources.find(id);

        if (it != resources.end()) {
            return it->second;
        }

        return load(id);
    }
};

#if RESOURCE_MANAGER_EXAMPLE
// @BAKE g++ -x c++ -DRESOURCE_MANAGER_EXAMPLE -o $*.out $@ -lraylib -Wall -Wpedantic -ggdb
#include <raylib.h>

signed main(void) {
    InitWindow(400, 300, "Resource Manager Example");

    Resource_manager<Texture2D> tms(
        LoadTexture,
        UnloadTexture,
        IsTextureValid,
        []() -> Texture2D {
            Texture2D r;

            Image img = GenImageColor(50, 50, BLACK);

            ImageDrawRectangle(&img,  0,  0, 25, 25, PURPLE);
            ImageDrawRectangle(&img, 25, 25, 25, 25, PURPLE);

            r = LoadTextureFromImage(img);
            UnloadImage(img);

            return r;
        }()
    );

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Default resource:", 20, 20, 20, BLACK);
        DrawTexture(tms.get("does-not-exist.png"), 20, 60, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
#endif
