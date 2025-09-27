<<<<<<< Updated upstream

CXX = g++
CXXFLAGS = -Wall -std=c++23
TARGETS = main
SRC_MAIN = ../src/main_server.cpp ../lib/libtslog.cpp

all: $(TARGETS)

main: $(SRC_MAIN)
	$(CXX) $(CXXFLAGS) -o main $(SRC_MAIN)

clean:
	del $(TARGETS)
=======
# Makefile para Servidor de Chat TCP - libtslog (com limpeza automática de objetos)
# Projeto: Programação Concorrente C++
# Autor: Seu Nome
# Data: 26/09/2025

# ==============================================================================
# CONFIGURAÇÕES DO COMPILADOR
# ==============================================================================
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

# ==============================================================================
# ESTRUTURA DE DIRETÓRIOS
# ==============================================================================
SRC_DIR = ../src
LIB_DIR = ../lib
OBJ_DIR = obj
BUILD_DIR = .

# ==============================================================================
# ARQUIVOS
# ==============================================================================
SOURCES = $(SRC_DIR)/libtslog.cpp $(SRC_DIR)/main_server.cpp
HEADERS = $(LIB_DIR)/libtslog.h $(LIB_DIR)/logEntry.h
OBJECTS = $(OBJ_DIR)/libtslog.o $(OBJ_DIR)/main_server.o
TARGET = chat_server

# ==============================================================================
# ALVOS PRINCIPAIS
# ==============================================================================
.PHONY: all clean clean-obj run help

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "🔗 Linkando executável: $@"
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@
	@$(MAKE) clean-obj
	@echo "✅ Compilação concluída com sucesso!"
	@echo "▶️  Execute com: ./$(TARGET)"

# ==============================================================================
# COMPILAÇÃO DE OBJETOS
# ==============================================================================
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "🔨 Compilando: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# ==============================================================================
# LIMPEZA
# ==============================================================================
clean: clean-obj
	rm -f $(TARGET)
	rm -f chat_server.log
	@echo "✅ Limpeza completa (executável, logs, objetos)"

clean-obj:
	rm -rf $(OBJ_DIR)
	@echo "✅ Objetos removidos ( $(OBJ_DIR) )"

# ==============================================================================
# EXECUÇÃO
# ==============================================================================
run: all
	@echo "🚀 Executando $(TARGET)..."
	./$(TARGET)

# ==============================================================================
# AJUDA
# ==============================================================================
help:
	@echo "Makefile - Servidor de Chat TCP"
	@echo "  all           - Compila o executável e limpa objetos"
	@echo "  run           - Compila e executa"
	@echo "  clean         - Remove executável, logs e objetos"
	@echo "  clean-obj     - Remove apenas objetos"
	@echo "  help          - Exibe esta ajuda"
>>>>>>> Stashed changes
