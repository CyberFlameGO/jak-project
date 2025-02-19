#pragma once

#include "common/goos/Interpreter.h"
#include "goalc/make/Tool.h"

struct MakeStep {
  std::vector<std::string> input;
  std::vector<std::string> deps, outputs;
  goos::Object arg;
  std::string tool;

  std::string print() const;
};

class MakeSystem {
 public:
  MakeSystem();
  void load_project_file(const std::string& file_path);

  goos::Object handle_defstep(const goos::Object& obj,
                              goos::Arguments& args,
                              const std::shared_ptr<goos::EnvironmentObject>& env);

  goos::Object handle_basename(const goos::Object& obj,
                               goos::Arguments& args,
                               const std::shared_ptr<goos::EnvironmentObject>& env);

  goos::Object handle_stem(const goos::Object& obj,
                           goos::Arguments&,
                           const std::shared_ptr<goos::EnvironmentObject>& env);

  std::vector<std::string> get_dependencies(const std::string& target) const;
  std::vector<std::string> filter_dependencies(const std::vector<std::string>& all_deps);

  bool make(const std::string& target, bool force, bool verbose);

  void add_tool(std::shared_ptr<Tool> tool);
  void set_constant(const std::string& name, const std::string& value);
  void set_constant(const std::string& name, bool value);

  template <typename T>
  void add_tool() {
    add_tool(std::make_shared<T>());
  }

  void clear_project();

 private:
  void va_check(const goos::Object& form,
                const goos::Arguments& args,
                const std::vector<std::optional<goos::ObjectType>>& unnamed,
                const std::unordered_map<std::string,
                                         std::pair<bool, std::optional<goos::ObjectType>>>& named);

  void get_dependencies(const std::string& master_target,
                        const std::string& output,
                        std::vector<std::string>* result_order,
                        std::unordered_set<std::string>* result_set) const;

  goos::Interpreter m_goos;

  std::unordered_map<std::string, std::shared_ptr<MakeStep>> m_output_to_step;
  std::unordered_map<std::string, std::shared_ptr<Tool>> m_tools;
};
