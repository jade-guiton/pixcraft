#include "window.hpp"

const char windowTitle[] = "OpenGL Test 2";

void windowResizeCallback(GLFWwindow* handle, int width, int height) {
	glfwMakeContextCurrent(handle);
	glViewport(0, 0, width, height);
	
	GameWindow* window = (GameWindow*) glfwGetWindowUserPointer(handle);
	window->width = width; window->height = height;
	
	std::cout << "Window resized to " << width << "×" << height << std::endl;
}

GameWindow::GameWindow() {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	handle = glfwCreateWindow(WINDOW_START_WIDTH, WINDOW_START_HEIGHT, windowTitle, NULL, NULL);
	if(handle == NULL)
		throw std::runtime_error("Failed to create GLFW window");
	glfwSetWindowUserPointer(handle, (void*) this);
	
	glfwMakeContextCurrent(handle);
	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
		throw std::runtime_error("Failed to initialize GLAD");
	glfwSetFramebufferSizeCallback(handle, windowResizeCallback);
	windowResizeCallback(handle, WINDOW_START_WIDTH, WINDOW_START_HEIGHT);
	
	glfwSwapInterval(-1);
	
	program = loadShaders();
	
	float vertices[] = {
		-0.5f, -0.5f,  0.5f,  
		 0.5f, -0.5f,  0.5f,  
		-0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,
	};
	unsigned int indices[] = {
		0, 2, 1,
		1, 2, 3
	};
	
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);
}

GameWindow::~GameWindow() {
	glfwDestroyWindow(handle);
}

bool GameWindow::shouldClose() {
	return glfwWindowShouldClose(handle);
}

void GameWindow::update() {
	glfwPollEvents();
	
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), ((float) width) / height, 0.001f, 1000.0f);
	
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
	
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	glfwSwapBuffers(handle);
}
