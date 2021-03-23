#include <filesystem>
#include <functional>
#include <queue>

#include <CLI/CLI.hpp>
#include <git2.h>

namespace fs = std::filesystem;

namespace list_files {

  using FilePredicate = std::function<bool(const fs::path&)>;

  struct Options {
    fs::path directory = ".";
    bool files_only = false;
    bool use_git_ignore = false;
  };

  struct GitIgnoreFilter {
    GitIgnoreFilter(const fs::path& path)
    {
      auto abs = fs::canonical(path);
      git_libgit2_init();
      int err = 0;
      err = git_repository_open_ext(&repo, abs.c_str(), 0, nullptr);
      if (err != 0) {
        throw std::runtime_error("Could not open git repository");
      }
    }
    ~GitIgnoreFilter()
    {
      git_repository_free(repo);
      git_libgit2_shutdown();
    }

    GitIgnoreFilter(const GitIgnoreFilter&) = delete;
    GitIgnoreFilter& operator=(const GitIgnoreFilter&) = delete;

    bool operator()(const fs::path& file)
    {
      auto p = fs::canonical(file);
      int res = 0;
      int err = git_ignore_path_is_ignored(&res, repo, p.c_str());
      if (err != 0) {
        throw std::runtime_error("Gitignore check failed");
        return true;
      }
      return res == 0;
    }

  private:
    ::git_repository* repo = nullptr;
  };

  void print_path(const fs::path& path, const Options& options)
  {
    if (options.files_only && fs::is_directory(path)) return;
    auto prox = fs::proximate(path);
    auto abs = fs::absolute(path);
    if (abs.string().size() <= prox.string().size()) {
      std::cout << abs.c_str() << '\n';
    } else {
      std::cout << prox.c_str() << '\n';
    }
  }

  void list_files(const Options& options)
  {
    list_files::FilePredicate pred = [](auto&&) { return true; };
    std::unique_ptr<list_files::GitIgnoreFilter> gitignore;

    if (options.use_git_ignore) {
      try {
        gitignore = std::make_unique<list_files::GitIgnoreFilter>(options.directory);
        pred = std::ref(*gitignore);
      } catch (std::exception& e) {
      }
    }

    if (!pred(options.directory)) return;
    std::queue<fs::path> queue;
    queue.push(options.directory);
    while (!queue.empty()) {
      auto next = queue.front();
      queue.pop();
      if (next != options.directory) {
        print_path(next, options);
      }
      if (fs::is_symlink(next)) continue;
      if (fs::is_directory(next)) {
        for (auto&& entry : fs::directory_iterator(next, fs::directory_options::none)) {
          if (pred(entry.path())) {
            queue.push(entry.path());
          }
        }
      }
    }
  }

} // namespace list_files

int main(int argc, char* argv[])
{
  list_files::Options options;
  CLI::App app{"List files breadth first"};
  app.add_option("directory", options.directory, "The directory to list files from");
  app.add_flag("-f,--files", options.files_only, "Only output files, not directories");
  app.add_flag("-g,--gitignore", options.use_git_ignore, "Skip files and folders ignored by git");

  CLI11_PARSE(app, argc, argv);

  list_files::list_files(options);
  return 0;
}
