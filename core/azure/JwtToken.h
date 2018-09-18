#ifndef JWT_TOKEN_H
#define JWT_TOKEN_H

#include <string>

namespace az
{
  class JwtToken
  {
    public:
      /**
       * @brief      Constructs the object.
       *
       * @param[in]  tokenType  The token type
       * @param[in]  token      The token
       */
      JwtToken(const std::string& tokenType = "", const std::string& token = "");

      /**
       * @brief      Destroys the object.
       */
      ~JwtToken();

      /**
       * @brief      { function_description }
       *
       * @return     { description_of_the_return_value }
       */
      std::string value() const;

      /**
       * @brief      { function_description }
       *
       * @return     { description_of_the_return_value }
       */
      std::string type() const;

    private:
      std::string m_tokenType;
      std::string m_token;
  };
} // az

#endif // JWT_TOKEN_H
