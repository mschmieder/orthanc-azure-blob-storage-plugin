#ifndef JWT_TOKEN_H
#define JWT_TOKEN_H

#include <cpprest/details/basic_types.h>

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
      JwtToken(const utility::string_t& tokenType = "", const utility::string_t& token = "");

      /**
       * @brief      Destroys the object.
       */
      ~JwtToken();

      /**
       * @brief      { function_description }
       *
       * @return     { description_of_the_return_value }
       */
      utility::string_t value() const;
      
      /**
       * @brief      { function_description }
       *
       * @return     { description_of_the_return_value }
       */
      utility::string_t type() const;

    private:
      utility::string_t m_tokenType;
      utility::string_t m_token;
  };
} // az

#endif // JWT_TOKEN_H
