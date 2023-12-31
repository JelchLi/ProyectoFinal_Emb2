#include "server.h"
#include "motors.h"
#include "sensores.h"
#include "cJSON.h"

//Definimos la memoria de la cola de transferencia del servidor, por defecto es 4096, esto no abarco asi que se decidio aumentar la memoria
#define HTTPD_STACK_SIZE 12228
#define HTTPD_MAX_URIS 9
//TAG para imprimir los datos por UART
static const char *TAG = "main";


//Handle que detiene el servidor
static void stop_webserver(httpd_handle_t server)
{
    // llama a la funcion que detiene el servidor
    httpd_stop(server);
}

//Handle para el inicio de la pagina 
static esp_err_t root_get_handler(httpd_req_t *req)
{
    //se calcula el tamaño del html para poder mandarlo como respuesta al cliente
    extern unsigned char view_start[] asm("_binary_view_html_start");
    extern unsigned char view_end[] asm("_binary_view_html_end");
    size_t view_len = view_end - view_start;
    char viewHtml[view_len];

    //mensaje de verificacion de ingreso al root
    ESP_LOGI(TAG, "root");

    //se guarda el html en la variable creada anteriormente 
    memcpy(viewHtml, view_start, view_len);

    //se manda la respuesta al cliente como tipo html
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, viewHtml, view_len);

    return ESP_OK;
}


static esp_err_t AForward_handler(httpd_req_t *req)
{
    //mensaje de verificacion 
    ESP_LOGI(TAG, "boton1");

    motorA_adelante();
    //se manda una respuesta nula, para que el navegador pueda continuar reciebiendo respuestas
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}


static esp_err_t ABackward_handler(httpd_req_t *req)
{
    //mensaje de verificacion 
    ESP_LOGI(TAG, "boton2");

    motorA_atras();
    //se manda una respuesta nula, para que el navegador pueda continuar reciebiendo respuestas
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}


static esp_err_t BForward_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "boton3");

    motorB_adelante();

    //se manda una respuesta nula, para que el navegador pueda continuar reciebiendo respuestas
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}


static esp_err_t BBackward_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "boton4");
    //se llama a la funcion para mover el motor hacia atras
    motorB_atras();

    //se manda una respuesta nula, para que el navegador pueda continuar recibiendo respuestas
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}


static esp_err_t reset_handler(httpd_req_t *req)
{
    
    ESP_LOGI(TAG, "restarted");
    //se llama a la funcion para mover el motor hacia atras
    reset_motors();

    //se manda una respuesta nula, para que el navegador pueda continuar recibiendo respuestas
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

//estructura creada para mandar al cliente en formato JSON
typedef struct {
    float temperatura;
    float presion;
    float corriente;
} SensorData;


//manejador de los datos de los sensores
static esp_err_t temp_handler(httpd_req_t *req)
{
    //mensaje de verificacion
    ESP_LOGI(TAG, "temperature sent");

    //se crea la estructura donde se guardaran los datos
    SensorData temp_data;
    temp_data.temperatura = get_temperature();

    //se crea un obejeto JSON donde se guardan los datos para enviar al cliente
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "temperatura", temp_data.temperatura);

    //convierte el objeto en una cadena para ser enviada 
    char *json_str = cJSON_PrintUnformatted(root);

    //se envia el objeto al cliente
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    //se libera el espacio utilizado por el objeto JSON
    free(json_str);
    cJSON_Delete(root);

    return ESP_OK;
}
//manejador de los datos de los sensores
static esp_err_t pressure_handler(httpd_req_t *req)
{
    //mensaje de verificacion
    ESP_LOGI(TAG, "pressure sent");

    //se crea la estructura donde se guardaran los datos
    SensorData pressure_data;
    pressure_data.temperatura = get_presion();

    //se crea un obejeto JSON donde se guardan los datos para enviar al cliente
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "presion", pressure_data.temperatura);

    //convierte el objeto en una cadena para ser enviada 
    char *json_str = cJSON_PrintUnformatted(root);

    //se envia el objeto al cliente
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    //se libera el espacio utilizado por el objeto JSON
    free(json_str);
    cJSON_Delete(root);

    return ESP_OK;
}
//manejador de los datos de los sensores
static esp_err_t current_handler(httpd_req_t *req)
{
    //mensaje de verificacion
    ESP_LOGI(TAG, "current sent");

    //se crea la estructura donde se guardaran los datos
    SensorData current_data;
    current_data.temperatura = get_corriente();

    //se crea un obejeto JSON donde se guardan los datos para enviar al cliente
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "corriente", current_data.temperatura);

    //convierte el objeto en una cadena para ser enviada 
    char *json_str = cJSON_PrintUnformatted(root);

    //se envia el objeto al cliente
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    //se libera el espacio utilizado por el objeto JSON
    free(json_str);
    cJSON_Delete(root);

    return ESP_OK;
}

//uri de la pagina inicial 
httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler
};

//uri del boton 1
httpd_uri_t motorAF = {
    .uri = "/motorAF",
    .method = HTTP_GET,
    .handler = AForward_handler
};

//uri del boton 2
httpd_uri_t motorAB = {
    .uri = "/motorAB",
    .method = HTTP_GET,
    .handler = ABackward_handler
};

//uri del boton 3
httpd_uri_t motorBF = {
    .uri = "/motorBF",
    .method = HTTP_GET,
    .handler = BForward_handler
};

//uri del boton 4
httpd_uri_t motorBB = {
    .uri = "/motorBB",
    .method = HTTP_GET,
    .handler = BBackward_handler
};

httpd_uri_t resetAB = {
    .uri = "/reset",
    .method = HTTP_GET,
    .handler = reset_handler
};

//uri de los sensores
httpd_uri_t temperatura = {
    .uri = "/temperatura",
    .method = HTTP_GET,
    .handler = temp_handler
};
//uri de los sensores
httpd_uri_t presion = {
    .uri = "/presion",
    .method = HTTP_GET,
    .handler = pressure_handler
};
//uri de los sensores
httpd_uri_t corriente = {
    .uri = "/corriente",
    .method = HTTP_GET,
    .handler = current_handler
};

//Inicia el servidor web
static httpd_handle_t start_webserver(void)
{

    //Declara la variable del servidor como vacia
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    //se utiliza la configuracion por default del servidor
    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

    //se cambia el tamano de envio de cola a 12228 bytes
    conf.stack_size = HTTPD_STACK_SIZE;

    //se aumenta el maximo de URI para registrar
    conf.max_uri_handlers = HTTPD_MAX_URIS;

    //se inicia el servidor con las configuraciones definidas
    esp_err_t ret = httpd_start(&server, &conf);

    //verificacion de la correcta inicializacion del servidor
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");

    //registro de todos los URI usados en el servidor
    httpd_register_uri_handler(server, &root);          //pagina inicial    
    httpd_register_uri_handler(server, &motorAF);       //motor A adelante
    httpd_register_uri_handler(server, &motorAB);       //motor A atras
    httpd_register_uri_handler(server, &motorBF);       //motor B adelante
    httpd_register_uri_handler(server, &motorBB);       //motor B atras
    httpd_register_uri_handler(server, &resetAB);       //reinicio de motores A y B
    httpd_register_uri_handler(server, &temperatura);   //temperatura
    httpd_register_uri_handler(server, &presion);       //presion
    httpd_register_uri_handler(server, &corriente);     //corriente

    //devuelve el servidor como respuesta
    return server;
}


//Manejadores de eventos para la conexion y desconexion del wifi
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
        //Informacion del estado de las conexiones
        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "conectando wifi...\n");
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "wifi conectado... \n");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "conexion perdida... \n");
            break;

        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP obtenido... \n\n");
            httpd_handle_t server = start_webserver();
            break;

        default:
            break;

    }
}

// Configuracion y conexion de la red wifi
void wifi_server_setup(void)
{
    //inicializa la interfaz de red para poder conectarse por wifi
    esp_netif_init();
    //se inicializan los eventos del sistema
    esp_event_loop_create_default();
    //se creaa una interfaz de red wifi en modo estacion 
    esp_netif_create_default_wifi_sta();

    //se configura la red con la configuracion por defecto 
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    //inicia el wifi 
    esp_wifi_init(&wifi_initiation);
    //registra los eventos generados por el wifi y al obtener el IP
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    //datos del wifi al que se conectara
    wifi_config_t wifi_configuration = 
    {
        .sta = 
        {
            .ssid = "Familia.L.P", 
            .password = "2352757Jamf"
        }
    };

    //configuracion en modo estacion
    esp_wifi_set_mode(WIFI_MODE_STA);
    //se configura el wifi 
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    //inicio del wifi 
    esp_wifi_start();
    //conecta al wifi 
    esp_wifi_connect();
    //espera para obtener el IP
    vTaskDelay(pdMS_TO_TICKS(3000));
}