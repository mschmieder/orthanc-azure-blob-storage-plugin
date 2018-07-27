#ifndef AUTH_ENDPOINT_H
#define AUTH_ENDPOINT_H

#include <cpprest/details/basic_types.h>

namespace az
{
  class AuthEndpoint
  {
    public:
      /**
       * @brief      Constructs the object.
       *
       * @param[in]  loginUri  The login uri
       * @param[in]  authUri   The auth uri
       */
      AuthEndpoint(const utility::string_t& loginUri = "",
                   const utility::string_t& authUri = "");

      /**
       * @brief      Destroys the object.
       */
      ~AuthEndpoint();

      /**
       * @brief      Gets the login uri.
       *
       * @return     The login uri.
       */
      utility::string_t getLoginUri() const;

      /**
       * @brief      Gets the auth uri.
       *
       * @return     The auth uri.
       */
      utility::string_t getAuthUri() const;

    private:
      utility::string_t m_loginUri;
      utility::string_t m_authUri;
  };
} // namespace az


#endif // AUTH_ENDPOINT_H
