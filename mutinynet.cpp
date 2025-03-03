#include <cpprest/http_client.h>

// using namespace utility;
using namespace web::json;
using namespace web::http;
using namespace web::http::client;
// using namespace concurrency::streams;

static web::http::client::http_client_config client_config_for_proxy()
{
    web::http::client::http_client_config client_config;
    if (const char* env_http_proxy = std::getenv("http_proxy")) {
        std::string env_http_proxy_string(env_http_proxy);
        if (env_http_proxy_string == U("auto")) {
            client_config.set_proxy(web::web_proxy::use_auto_discovery);
        } else {
            client_config.set_proxy(web::web_proxy(env_http_proxy_string));
        }
    }

    return client_config;
}

static web::json::value request(const char *url)
{
    web::json::value res;

    http_client client(U(url), client_config_for_proxy());
    client.request(methods::GET)

    .then([&res](http_response response) {
        if (response.status_code() != status_codes::OK) {
            printf("Response status code %u returned.\n", response.status_code());
        }
        return response.extract_json();
    })

    .then([&res](web::json::value response) {
        res = response;
    })

    // Wait for the entire response body to be written into the file.
    .wait();

    return res;
}

int mutiny_address_utxo_value(const char *address)
{
    int ret = 0;
    auto url = "https://mutinynet.com/api/address/" + std::string(address) + "/utxo";
    auto res = request(url.c_str());
    if (res.is_array() && res.size() > 0) {
        int sum = 0;
        for (auto& iter : res.as_array()) {
            sum += iter["value"].as_integer();
            // printf("value=%d\n", sum);
        }
        ret = sum;
    }
    return ret;
}

bool mutiny_detect_address(const char *address)
{
    auto url = "https://mutinynet.com/api/address/" + std::string(address);
    auto res = request(url.c_str());
    return res["chain_stats"]["tx_count"].as_integer() > 0;
}
