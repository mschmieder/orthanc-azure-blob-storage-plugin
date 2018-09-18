#ifndef AUTH_ENDPOINT_H
#define AUTH_ENDPOINT_H

#include <string>

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
      AuthEndpoint(const std::string& loginUri = "",
                   const std::string& authUri = "");

      /**
       * @brief      Destroys the object.
       */
      ~AuthEndpoint();

      /**
       * @brief      Gets the login uri.
       *
       * @return     The login uri.
       */
      std::string getLoginUri() const;

      /**
       * @brief      Gets the auth uri.
       *
       * @return     The auth uri.
       */
      std::string getAuthUri() const;

    private:
      std::string m_loginUri;
      std::string m_authUri;
  };
} // namespace az


#endif // AUTH_ENDPOINT_H
