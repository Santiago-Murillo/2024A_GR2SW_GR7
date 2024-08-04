#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/CubeMap.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

struct AABB { // Estructura AABB para caja de colisiones
    glm::vec3 min;
    glm::vec3 max;

    AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
};

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec) {
    os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
    return os;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

//void calculateRainbowColor(float time, float& r, float& g, float& b);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 3.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool flashlightTurnedOn = true;
bool FClicked = false;

// Coordenadas de paredes de la nave para colisión
std::vector<AABB> collisionBoxes = {
    AABB(glm::vec3(-2.5f, 3.0f, -35.0f), glm::vec3(2.5f, 6.0f, -33.0f)),
    AABB(glm::vec3(2.5f, 3.0f, -35.0f), glm::vec3(4.5f, 6.0f, -13.0f)),
    AABB(glm::vec3(4.5f, 3.0f, -15.0f), glm::vec3(6.5f, 6.0f, -13.0f)),
    AABB(glm::vec3(6.5f, 3.0f, -15.0f), glm::vec3(8.5f, 6.0f, -6.5f)),
    AABB(glm::vec3(4.5f, 3.0f, -8.5f), glm::vec3(6.5f, 6.0f, -6.5f)),
    AABB(glm::vec3(2.5f, 3.0f, -8.5f), glm::vec3(4.5f, 6.0f, 12.0f)),
    AABB(glm::vec3(4.5f, 3.0f, 10.0f), glm::vec3(14.0f, 6.0f, 12.0f)),
    AABB(glm::vec3(14.0f, 3.0f, 10.0f), glm::vec3(16.0f, 6.0f, 19.5f)),
    AABB(glm::vec3(4.5f, 3.0f, 17.5f), glm::vec3(14.0f, 6.0f, 19.5f)),
    AABB(glm::vec3(2.5f, 3.0f, 17.5f), glm::vec3(4.5f, 6.0f, 38.5f)),
    AABB(glm::vec3(-2.5f, 3.0f, 36.5f), glm::vec3(2.5f, 6.0f, 38.5f)),
    AABB(glm::vec3(-4.5f, 3.0f, 12.5f), glm::vec3(-2.5f, 6.0f, 38.5f)),
    AABB(glm::vec3(-6.5f, 3.0f, 12.5f), glm::vec3(-4.5f, 6.0f, 14.5f)),
    AABB(glm::vec3(-8.5f, 3.0f, 6.0f), glm::vec3(-6.5f, 6.0f, 14.5f)),
    AABB(glm::vec3(-6.5f, 3.0f, 6.0f), glm::vec3(-4.5f, 6.0f, 8.0f)),
    AABB(glm::vec3(-4.5f, 3.0f, -12.5f), glm::vec3(-2.5f, 6.0f, 8.0f)),
    AABB(glm::vec3(-14.0f, 3.0f, -12.5f), glm::vec3(-4.5f, 6.0f, -10.5f)),
    AABB(glm::vec3(-16.0f, 3.0f, -20.0f), glm::vec3(-14.0f, 6.0f, -10.5f)),
    AABB(glm::vec3(-14.0f, 3.0f, -20.0f), glm::vec3(-4.5f, 6.0f, -18.0f)),
    AABB(glm::vec3(-4.5f, 3.0f, -35.0f), glm::vec3(-2.5f, 6.0f, -18.0f))
};

// Método para verificar colisiones.
bool checkCollision(const glm::vec3& position) {
    for (const auto& box : collisionBoxes) {
        if ((position.x >= box.min.x && position.x <= box.max.x) &&
            (position.z >= box.min.z && position.z <= box.max.z)) {
            return true;
        }
    }
    return false;
}

unsigned int loadCubemap(std::vector<std::string> faces);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Remove window decorations34

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Get primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final SAJE", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set window position to cover the entire screen
    glfwSetWindowPos(window, 0, 0);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    // Corregir gamma (se vuelve mas brillante)
    //glEnable(GL_FRAMEBUFFER_SRGB);

    // Cargar las texturas del cubemap
    std::vector<std::string> cubeMapFaces
    {
        "images/right3.jpg",
        "images/left3.jpg",
        "images/top3.jpg",
        "images/bottom3.jpg",
        "images/front3.jpg",
        "images/back3.jpg"
    };
    CubeMap cubeMap(cubeMapFaces);

    // build and compile shaders
    // -------------------------
    Shader modelShader("shaders/shader_modelo.vs", "shaders/shader_modelo.fs");
    Shader skyboxShader("shaders/shader_skybox.vs", "shaders/shader_skybox.fs");
    Shader lightShader("shaders/shader_emisorLuz.vs", "shaders/shader_emisorLuz.fs");

    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.0f,  4.0f,  -3.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    // load models
    // -----------
    Model scifi_hallway("model/scifi_hallway/scifi_hallway.obj");
    Model drone("model/drone/drone.obj");
    Model sol("model/sol/Sol.obj");
    Model mercurio("model/mercurio/mercurio.obj");
    Model venus("model/venus/venus.obj");
    Model tierra("model/tierra/tierra.obj");
    Model marte("model/marte/marte.obj");
    Model jupiter("model/jupiter/jupiter.obj");
    Model saturn("model/saturn/saturno.obj");
    Model urano("model/urano/urano.obj");
    Model neptuno("model/neptuno/neptuno.obj");
  
    modelShader.use(); 
    modelShader.setInt("material.diffuse", 0);
    modelShader.setInt("material.specular", 1);
    modelShader.setInt("material.emission", 3);
  
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 10; //Optional. Modify the speed of the camera

    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // Clear color and depth buffer
        float timeValue = glfwGetTime();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render cubemap
        glm::mat4 viewSkybox = glm::mat4(glm::mat3(camera.GetViewMatrix())); // Eliminar la traslaci�n

        // Calculate the rotation angle
        float angle = currentFrame * glm::radians(0.5f); // Rotate x degrees per second
        viewSkybox = glm::rotate(viewSkybox, angle, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate around the Y axis

        /// Escalar el cubemap
        glm::mat4 modelSkybox = glm::mat4(1.0f);
        modelSkybox = glm::scale(modelSkybox, glm::vec3(1000000.0f, 1000000.0f, 1000000.0f)); // Ajustar la escala del cubemap

        glm::mat4 projectionSkybox = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000000.0f);
        skyboxShader.use();
        skyboxShader.setMat4("model", modelSkybox); // Enviar la matriz de modelo al shader
        skyboxShader.setMat4("view", viewSkybox);
        skyboxShader.setMat4("projection", projectionSkybox);
        cubeMap.render(skyboxShader, viewSkybox, projectionSkybox);

        // Renderizar point lights
        lightShader.use();


// Renderizar los modelos
        // ---------------------
        modelShader.use();
    // Configurar luces
        modelShader.setVec3("viewPos", camera.Position);
        modelShader.setFloat("material.shininess", 32.0f);

        //// directional light
        modelShader.setVec3("dirLight.direction", -0.2f, 1.0f, 1.0f);
        modelShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        modelShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light 1
        modelShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        modelShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        modelShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        modelShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        
        // modelShader.setFloat("pointLights[0].constant", 1.0f);
        // modelShader.setFloat("pointLights[0].linear", 0.22);
        // modelShader.setFloat("pointLights[0].quadratic", 0.20);
        modelShader.setFloat("pointLights[0].constant", 1.0f);
        modelShader.setFloat("pointLights[0].linear", 0.09);
        modelShader.setFloat("pointLights[0].quadratic", 0.032);

        // flashlight (spotlight)
        modelShader.setVec3("spotLight.position", camera.Position);
        modelShader.setVec3("spotLight.direction", camera.Front);

        if (flashlightTurnedOn)
        {
            modelShader.setVec3("spotLight.ambient", 1.0f, 1.0f, 1.0f);
            modelShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        }
        else
        {
            modelShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }

        modelShader.setFloat("spotLight.constant", 1.0f);
        modelShader.setFloat("spotLight.linear", 0.09);
        modelShader.setFloat("spotLight.quadratic", 0.032);
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
      
    // Configurar matrices
        glm::mat4 projectionModel = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000000.0f);
        glm::mat4 viewModel = camera.GetViewMatrix();

        modelShader.setMat4("projection", projectionModel);
        modelShader.setMat4("view", viewModel);


        // render scifi_hallway
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader.setMat4("model", model);
        scifi_hallway.Draw(modelShader);

        // render Sol
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3600.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(52.0f, 52.0f, 52.0f)); // Ajustar la escala del Sol
        modelShader.setMat4("model", model);
        sol.Draw(modelShader);

        //render de mercurio 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 5000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Ajustar la escala de mercurio
        modelShader.setMat4("model", model);
        mercurio.Draw(modelShader);

        //render de venus 
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 10000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f)); // Ajustar la escala de venus
        modelShader.setMat4("model", model);
        venus.Draw(modelShader);

        //render de la tierra
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 20000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(70.2f, 70.2f, 70.2f)); // Ajustar la escala de la tierra
        modelShader.setMat4("model", model);
        tierra.Draw(modelShader);

        //render de marte
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 30000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f)); // Ajustar la escala de marte
        modelShader.setMat4("model", model);
        marte.Draw(modelShader);

        //render de jupiter
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 40000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(150.5f, 150.5f, 150.5f)); // Ajustar la escala de jupiter
        modelShader.setMat4("model", model);
        jupiter.Draw(modelShader);

        //render de saturno
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 50000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(650.0f, 650.0f, 650.0f)); // Ajustar la escala de saturno
        modelShader.setMat4("model", model);
        saturn.Draw(modelShader);

        //render de urano
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 60000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(0.43f, 0.43f, 0.43f)); // Ajustar la escala de urano
        modelShader.setMat4("model", model);
        urano.Draw(modelShader);

        //render de neptuno
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3500.0f + 80000.0f, 3000.0f, -3000.0f)); // Posición ajustada para estar lejos del dron y la nave
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); // Ajustar la escala de neptuno
        modelShader.setMat4("model", model);
        neptuno.Draw(modelShader);

        //// render far scifi_hallway
        //model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(150.0f, 30.0f, -150.0f));
        //model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        //modelShader.setMat4("model", model);
        //scifi_hallway.Draw(modelShader);


        // render drone
            // Forma medio trucada para rotar en base a un eje, funciona raro, se recomienda buscar otra foma:
        float radioDron = 25.0f;
        glm::vec3 axis(0.0f, 0.0f, 1.0f); // Ajusta este vector seg�n sea necesario
        glm::vec3 center(-7.0f, 0.0f, 0.0f); // Ajusta el centro seg�n sea necesario
        axis = glm::normalize(axis);

        model = glm::mat4(1.0f);
        // Trasladar el modelo al centro de rotaci�n
        model = glm::translate(model, center);
        // Rotar alrededor del eje arbitrario
        model = glm::rotate(model, timeValue / 2, axis);
        // Trasladar el modelo al radio deseado desde el centro de rotaci�n
        model = glm::translate(model, glm::vec3(radioDron, 0.0f, 0.0f));

        model = glm::scale(model, glm::vec3(-0.5f, 0.5f, 0.5f));
        modelShader.setMat4("model", model);
        drone.Draw(modelShader);
       
        // ------- IMPRIMIR POSICIÓN ACTUAL DE LA CÁMARA
        std::ostringstream oss;
        oss << "position: " << camera.Position;
        std::cout << oss.str() << std::endl;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    glm::vec3 prevPosition = camera.Position;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(RUN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        camera.ProcessKeyboard(WALK, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)

    {
    // Desconmentando esto y comendanto lo otro se arregla el bug de la linterna pero queda interesante xd
        //if (!FClicked)
        //{
        //    FClicked = true; // Marcar que se ha hecho clic
        //    flashlightTurnedOn = !flashlightTurnedOn; // Cambiar el estado de la linterna
        //}
        FClicked = true; // Marcar que se ha hecho clic
        flashlightTurnedOn = !flashlightTurnedOn; // Cambiar el estado de la linterna
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        FClicked = false; // Reiniciar la variable cuando se suelte el bot?n del mouse
    }
    //camera.Position.y = 3.0f;

    if (checkCollision(camera.Position)) {
        // Limitar la posición de la cámara a los límites de la caja de colisión
        std::cout << "------- COLISION DETECTADA -------" << std::endl;
        camera.Position = prevPosition;
        camera.Position.y = 3.0f;
    }

    //camera.Position.y = 3.0f;

    //mover la camara al planeta

    // 1
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        // mover al sol
        camera.Position = glm::vec3(1069.51f, 2885.28f, -4001.84f);
        camera.MovementSpeed = 1000.0f;
    }
    // 2
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        // mover a mercurio
        camera.Position = glm::vec3(8478.86f, 2992.59f, -3040.43f);
        camera.MovementSpeed = 500.0f;
    }

    // 3
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        // mover a venus
        camera.Position = glm::vec3(13484.5f, 3010.7f, -3063.85f);
        camera.MovementSpeed = 500.0f;
    }

    // 4
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        // mover a tierra
        camera.Position = glm::vec3(23455.5f, 3003.19f, -2994.04f);
        camera.MovementSpeed = 500.0f;
    }

    // 5
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        // mover a marte
        camera.Position = glm::vec3(33489.2f, 3009.61f, -3026.56f);
        camera.MovementSpeed = 500.0f;
    }

    // 6
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        // mover a jupiter
        camera.Position = glm::vec3(43380.6f, 3049.45f, -3395.93f);
        camera.MovementSpeed = 850.0f;
    }

    // 7
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        // mover a saturno
        camera.Position = glm::vec3(52656.1f, 3117.08f, -2911.97f);
        camera.MovementSpeed = 800.0f;
    }

    // 8
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        // mover a urano
        camera.Position = glm::vec3(63101.7f, 2967.07f, -2832.81f);
        camera.MovementSpeed = 800.0f;
    }

    // 9
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        // Mover a neptuno
        camera.Position = glm::vec3(83689.2f, 3034.89f, -2672.7f);
        camera.MovementSpeed = 800.0f;
    }
    // 0
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
    {
		// mover a nave
		camera.Position = glm::vec3(-0.651043f, 3.10671f, 1.98734f);
        camera.MovementSpeed = 10.0f;
	}
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        
        camera.MovementSpeed = 10000.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {

        camera.MovementSpeed = 1000.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    {

        camera.MovementSpeed = 100.0f;
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// void calculateRainbowColor(float time, float& r, float& g, float& b) {
//     float frequency = 1.0f; // Ajusta la frecuencia para controlar la velocidad del cambio de color
//     float phase = time * frequency;

//     // Sinusoides para los colores base del arco�ris
//     r = std::sin(phase + 0.0f) * 0.5f + 0.5f;
//     g = std::sin(phase + 2.0f) * 0.5f + 0.5f;
//     b = std::sin(phase + 4.0f) * 0.5f + 0.5f;

//     // Variar la intensidad para pasar por negro y blanco
//     float intensity = std::sin(phase * 1.0f) * 0.5f + 0.5f;
//     r *= intensity;
//     g *= intensity;
//     b *= intensity;
// }

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}