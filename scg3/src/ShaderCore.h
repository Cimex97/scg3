/**
 * \file ShaderCore.h
 * \brief A core to set a shader program to be applied to subsequent nodes.
 *
 * Defines struct:
 *   ShaderID
 *
 * \author Volker Ahlers\n
 *         volker.ahlers@fh-hannover.de
 */

#ifndef SHADERCORE_H_
#define SHADERCORE_H_

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>
#include "scg_glew.h"
#include "Core.h"
#include "scg_internals.h"

namespace scg {


/**
 * \brief A shader ID to identify shaders in error messages.
 * An arbitrary name can be added to the OpnGL shader index, e.g., a file name.
 */
struct ShaderID {

  ShaderID()
      : shader(0) {
  }

  ShaderID(GLuint shader0, const std::string& name0)
      : shader(shader0), name(name0) {
  }

  GLuint shader;
  std::string name;

};


/**
 * \brief A core to set a shader program to be applied to subsequent nodes.
 *
 * A few member functions are defined in the header file to allow inlining.
 * Method chaining (via returning this pointers) is not supported to ensure maximum
 * performance.
 */
class ShaderCore: public Core {

public:

  /**
   * Constructor with given program and shaders.
   */
  ShaderCore(GLuint program, const std::vector<ShaderID>& shaderIDs);

  /**
   * Destructor.
   */
  virtual ~ShaderCore();

  /**
   * Delete shaders and program.
   */
  void clear();

  /**
   * Create shared pointer.
   */
  static ShaderCoreSP create(GLuint program, const std::vector<ShaderID>& shaderIDs);

  /**
   * Initialize shader core, i.e., compile shaders and link program,
   * to be called by ShaderCoreFactory or by application after binding
   * custom attribute and fragment data locations.
   */
  void init() const;

  /**
   * Get shader program.
   */
  GLuint getProgram() const;

  /**
   * Get location of uniform variable.
   */
  GLint getUniformLoc(const std::string& name) const {
    assert(program_ != 0);
    auto it = uniformLocMap_.find(name);
    if (it == uniformLocMap_.end()) {
      it = uniformLocMap_.insert(it,
          std::make_pair(name, glGetUniformLocation(program_, name.c_str())));
    }
    return it->second;
  }

  /**
   * Set uniform variable.
   */
  void setUniform1i(const std::string& name, GLint value) const {
    glProgramUniform1i(program_, getUniformLoc(name), value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform1iv(const std::string& name, GLsizei count, const GLint* value) const {
    glProgramUniform1iv(program_, getUniformLoc(name), count, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform1f(const std::string& name, GLfloat value) const {
    glProgramUniform1f(program_, getUniformLoc(name), value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform1fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniform1fv(program_, getUniformLoc(name), count, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform2fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniform2fv(program_, getUniformLoc(name), count, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform3fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniform3fv(program_, getUniformLoc(name), count, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniform4fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniform4fv(program_, getUniformLoc(name), count, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniformMatrix2fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniformMatrix2fv(program_, getUniformLoc(name), count, GL_FALSE, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniformMatrix3fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniformMatrix3fv(program_, getUniformLoc(name), count, GL_FALSE, value);
  }

  /**
   * Set uniform variable.
   */
  void setUniformMatrix4fv(const std::string& name, GLsizei count, const GLfloat* value) const {
    glProgramUniformMatrix4fv(program_, getUniformLoc(name), count, GL_FALSE, value);
  }

  /**
   * Render shader, i.e., bind shader program.
   */
  virtual void render(RenderState* renderState);

  /**
   * Render shader after traversing sub-tree, i.e., restore previous shader program.
   */
  virtual void renderPost(RenderState* renderState);

protected:

  /**
   * Check for compile errors and print error messages.
   */
  void checkCompileError_(const ShaderID& shaderID) const;

  /**
   * Check for link errors and print error messages.
   */
  void checkLinkError_(GLuint program) const;

protected:

  GLuint program_;
  GLint programOld_;
  std::vector<ShaderID> shaderIDs_;
  ShaderCore* shaderCoreOld_;
  mutable std::unordered_map<std::string, GLint> uniformLocMap_;

};


} /* namespace scg */

#endif /* SHADERCORE_H_ */