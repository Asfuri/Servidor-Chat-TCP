# ==============================================================================
# CONFIGURAÇÕES DO COMPILADOR
# ==============================================================================
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -pthread

# ==============================================================================
# ESTRUTURA DE DIRETÓRIOS
# ==============================================================================
SRC_DIR = ../src
SCRIPTS_DIR = ../scripts
LIB_DIR = ../lib
OBJ_DIR = obj
LOG_DIR = logs
BUILD_DIR = .

# ==============================================================================
# ARQUIVOS E ALVOS
# ==============================================================================
HEADERS = $(LIB_DIR)/libtslog.h $(LIB_DIR)/logEntry.h $(LIB_DIR)/message_history.h

# Executáveis
SYNC_TEST = test_sync_clients
TEST_LIBTSLOG = test_libtslog
TCP_SERVER = tcp_server
TCP_CLIENT = tcp_client

# Arquivos objeto
LIBTSLOG_OBJ = $(OBJ_DIR)/libtslog.o
TEST_LIBTSLOG_OBJ = $(OBJ_DIR)/test_libtslog.o
SYNC_TEST_OBJ = $(OBJ_DIR)/test_sync_clients.o
TCP_SERVER_OBJ = $(OBJ_DIR)/tcp_server.o
TCP_CLIENT_OBJ = $(OBJ_DIR)/tcp_client.o
MESSAGE_HISTORY_OBJ = $(OBJ_DIR)/message_history.o

# Arquivos de log na pasta logs/
TEST_LOG = $(LOG_DIR)/chat_server.log
SERVER_LOG = $(LOG_DIR)/server.log
CLIENT_LOG = $(LOG_DIR)/client.log

# ==============================================================================
# ALVOS PRINCIPAIS
# ==============================================================================
.PHONY: all clean clean-obj clean-logs run-test run-server run-client test-tcp help setup

# Compila todos os executáveis e cria estrutura
all: setup $(TEST_LIBTSLOG) $(TCP_SERVER) $(TCP_CLIENT)
	@echo "✅ Compilação completa!"
	@echo "📦 Executáveis disponíveis:"
	@echo "   ./$(TEST_LIBTSLOG)  - Teste da biblioteca libtslog"
	@echo "   ./$(TCP_SERVER)     - Servidor TCP de Chat"
	@echo "   ./$(TCP_CLIENT)     - Cliente CLI de Chat"
	@echo "📁 Logs serão salvos em: $(LOG_DIR)/"

# Cria diretórios necessários
setup:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LOG_DIR)
	@echo "📁 Diretórios criados: $(OBJ_DIR)/, $(LOG_DIR)/"

# ==============================================================================
# COMPILAÇÃO DOS EXECUTÁVEIS
# ==============================================================================

# Teste da biblioteca libtslog
$(TEST_LIBTSLOG): $(LIBTSLOG_OBJ) $(TEST_LIBTSLOG_OBJ)
	@echo "🔗 Linkando teste da libtslog: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilar teste sincronizado
$(SYNC_TEST): $(SYNC_TEST_OBJ)
	@echo "🔗 Linkando teste sincronizado: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# Servidor TCP de Chat
$(TCP_SERVER): $(LIBTSLOG_OBJ) $(MESSAGE_HISTORY_OBJ) $(TCP_SERVER_OBJ)
	@echo "🔗 Linkando servidor TCP: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# Cliente CLI de Chat
$(TCP_CLIENT): $(LIBTSLOG_OBJ) $(TCP_CLIENT_OBJ)
	@echo "🔗 Linkando cliente TCP: $@"
	$(CXX) $(CXXFLAGS) $^ -o $@

# ==============================================================================
# COMPILAÇÃO DE OBJETOS
# ==============================================================================

# Biblioteca libtslog
$(LIBTSLOG_OBJ): $(SRC_DIR)/libtslog.cpp $(HEADERS) | setup
	@echo "🔨 Compilando libtslog: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# Teste da libtslog (renomeado do main_server.cpp)
$(TEST_LIBTSLOG_OBJ): $(SRC_DIR)/test_libtslog.cpp $(HEADERS) | setup
	@echo "🔨 Compilando teste libtslog: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# Servidor TCP
$(TCP_SERVER_OBJ): $(SRC_DIR)/tcp_server.cpp $(HEADERS) | setup
	@echo "🔨 Compilando servidor TCP: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# Cliente TCP
$(TCP_CLIENT_OBJ): $(SRC_DIR)/tcp_client.cpp | setup
	@echo "🔨 Compilando cliente TCP: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

$(MESSAGE_HISTORY_OBJ): $(SRC_DIR)/message_history.cpp $(LIB_DIR)/message_history.h | setup
	@echo "🔨 Compilando histórico de mensagens: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

$(SYNC_TEST_OBJ): $(SCRIPTS_DIR)/test_sync_clients.cpp | setup
	@echo "🔨 Compilando teste sincronizado: $<"
	$(CXX) $(CXXFLAGS) -I$(LIB_DIR) -c $< -o $@

# ==============================================================================
# EXECUÇÃO COM LOGS ORGANIZADOS
# ==============================================================================

# Executa teste da libtslog
run-test: $(TEST_LIBTSLOG) setup
	@echo "🧪 Executando teste da libtslog..."
	@echo "📝 Logs serão salvos em: $(TEST_LOG)"
	@echo "----------------------------------------"
	./$(TEST_LIBTSLOG)
	@echo "----------------------------------------"
	@if [ -f $(TEST_LOG) ]; then \
		echo "📊 Linhas no log: $$(wc -l < $(TEST_LOG))"; \
		echo "🔍 Primeiras 5 linhas:"; \
		head -5 $(TEST_LOG); \
		echo "📄 Log completo disponível em: $(TEST_LOG)"; \
	else \
		echo "⚠️  Arquivo de log não encontrado em $(TEST_LOG)"; \
	fi

# Executa servidor TCP
run-server: $(TCP_SERVER) setup
	@echo "🚀 Iniciando servidor TCP na porta 8080..."
	@echo "📝 Logs do servidor em: $(SERVER_LOG)"
	@echo "💡 Para conectar: use 'make run-client' em outro terminal"
	@echo "========================================="
	./$(TCP_SERVER)

# Executa cliente TCP
run-client: $(TCP_CLIENT) setup
	@echo "📱 Iniciando cliente TCP..."
	@echo "📝 Logs do cliente em: $(CLIENT_LOG)"
	@echo "💡 Conectando ao servidor localhost:8080"
	@echo "💡 Digite mensagens ou 'sair' para encerrar"
	@echo "========================================="
	./$(TCP_CLIENT)

# Executa cliente com IP/porta customizados
run-client-custom: $(TCP_CLIENT) setup
	@echo "📱 Uso: make run-client-custom SERVER_IP=127.0.0.1 SERVER_PORT=8080"
	@if [ -z "$(SERVER_IP)" ]; then \
		echo "❌ Defina SERVER_IP (ex: SERVER_IP=192.168.1.100)"; \
		exit 1; \
	fi
	@if [ -z "$(SERVER_PORT)" ]; then \
		echo "❌ Defina SERVER_PORT (ex: SERVER_PORT=8080)"; \
		exit 1; \
	fi
	@echo "🔗 Conectando a $(SERVER_IP):$(SERVER_PORT)"
	@echo "📝 Logs em: $(CLIENT_LOG)"
	./$(TCP_CLIENT) $(SERVER_IP) $(SERVER_PORT)

# ==============================================================================
# TESTES AUTOMATIZADOS COM LOGS ORGANIZADOS
# ==============================================================================

# Teste completo do sistema TCP
test-tcp: $(TCP_SERVER) $(TCP_CLIENT) setup
	@echo "⚡ Iniciando teste automatizado TCP"
	@echo "📁 Logs em: $(LOG_DIR)/"
	@echo "📡 Iniciando servidor em background..."
	@./$(TCP_SERVER) & echo $$! > $(LOG_DIR)/server.pid
	@sleep 1
	@echo "👥 Conectando múltiplos clientes..."
	@for i in 1 2 3; do \
		echo "🤖 Cliente $$i: conectando..."; \
		( \
			sleep 2; \
			echo "Mensagem automática do cliente $$i"; \
			sleep 1; \
			echo "sair" \
		) | ./$(TCP_CLIENT) 127.0.0.1 8080 > $(LOG_DIR)/client_$$i.log 2>&1 & \
	done
	@sleep 5
	@echo "🛑 Parando servidor..."
	@if [ -f $(LOG_DIR)/server.pid ]; then \
		kill `cat $(LOG_DIR)/server.pid` 2>/dev/null || true; \
		rm -f $(LOG_DIR)/server.pid; \
	fi
	@echo "📊 Verificando logs do servidor..."
	@if [ -f $(SERVER_LOG) ]; then \
		echo "✅ Logs do servidor ($(SERVER_LOG)):"; \
		echo "   Linhas: $$(wc -l < $(SERVER_LOG))"; \
		echo "   Conexões: $$(grep -c 'conectado' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
		echo "   Mensagens: $$(grep -c 'Mensagem recebida' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
		echo "   Retransmissões: $$(grep -c 'Mensagem retransmitida' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
	else \
		echo "❌ Arquivo $(SERVER_LOG) não encontrado"; \
	fi
	@echo "📁 Todos os logs em: $(LOG_DIR)/"
	@ls -la $(LOG_DIR)/


# Teste de stress com logs organizados
stress-test: $(TCP_SERVER) $(SYNC_TEST) setup
	@echo "⚡ Teste com sincronização via Barrier (condition_variable)"
	@echo "📁 Logs em: $(LOG_DIR)/"
	@./$(TCP_SERVER) > $(LOG_DIR)/server_sync.log 2>&1 & echo $$! > $(LOG_DIR)/server.pid
	@sleep 1  # Apenas para servidor subir
	@echo "👥 Iniciando clientes sincronizados..."
	./$(SYNC_TEST) 5  # 5 clientes com barrier (SEM SLEEP - usa barreiras!)
	@if [ -f $(LOG_DIR)/server.pid ]; then \
		kill `cat $(LOG_DIR)/server.pid` 2>/dev/null || true; \
		rm -f $(LOG_DIR)/server.pid; \
	fi
	@echo "📊 Resultado:"
	@if [ -f $(SERVER_LOG) ]; then \
		echo "   Conexões: $$(grep -c 'conectado' $(SERVER_LOG))"; \
		echo "   Mensagens: $$(grep -c 'Mensagem recebida' $(SERVER_LOG))"; \
	fi
	@echo "✅ Teste sincronizado concluído"

# ==============================================================================
# ANÁLISE DE LOGS
# ==============================================================================

# Mostra resumo de todos os logs
logs-summary: setup
	@echo "📊 RESUMO DOS LOGS"
	@echo "=================="
	@if [ -d $(LOG_DIR) ]; then \
		echo "📁 Diretório de logs: $(LOG_DIR)/"; \
		echo "📄 Arquivos encontrados:"; \
		ls -lah $(LOG_DIR)/ 2>/dev/null || echo "   (vazio)"; \
		echo ""; \
		for log in $(LOG_DIR)/*.log; do \
			if [ -f "$$log" ]; then \
				echo "📄 $$(basename $$log):"; \
				echo "   Linhas: $$(wc -l < $$log)"; \
				echo "   Tamanho: $$(du -h $$log | cut -f1)"; \
				echo "   Última modificação: $$(stat -c %y $$log 2>/dev/null || stat -f %Sm $$log 2>/dev/null || echo 'N/A')"; \
				echo ""; \
			fi; \
		done; \
	else \
		echo "❌ Diretório $(LOG_DIR) não encontrado"; \
	fi

# Visualiza logs em tempo real
logs-tail:
	@echo "👀 Monitorando logs em tempo real..."
	@echo "💡 Pressione Ctrl+C para parar"
	@if [ -f $(SERVER_LOG) ]; then \
		echo "📡 Monitorando: $(SERVER_LOG)"; \
		tail -f $(SERVER_LOG); \
	else \
		echo "⚠️  Arquivo $(SERVER_LOG) não encontrado"; \
		echo "🚀 Inicie o servidor primeiro: make run-server"; \
	fi

# Limpa apenas logs antigos (mantém estrutura)
clean-logs:
	@echo "🧹 Limpando logs antigos..."
	@if [ -d $(LOG_DIR) ]; then \
		rm -f $(LOG_DIR)/*.log $(LOG_DIR)/*.pid; \
		rm -rf $(LOG_DIR)/stress; \
		echo "✅ Logs limpos (diretório mantido)"; \
	else \
		echo "📁 Diretório $(LOG_DIR) não existe"; \
	fi

# ==============================================================================
# LIMPEZA
# ==============================================================================

# Limpeza completa (mantém pasta logs vazia)
clean: clean-obj
	@echo "🧹 Limpando executáveis..."
	rm -f $(TEST_LIBTSLOG) $(TCP_SERVER) $(TCP_CLIENT) $(SYNC_TEST)
	@$(MAKE) clean-logs
	@echo "✅ Limpeza completa ($(LOG_DIR)/ mantido vazio)"

# Limpeza apenas de objetos
clean-obj:
	@echo "🧹 Limpando objetos..."
	rm -rf $(OBJ_DIR)

# Limpeza total (remove até pasta logs)
clean-all: clean
	@echo "🧹 Removendo pasta de logs..."
	rm -rf $(LOG_DIR)
	@echo "✅ Limpeza total concluída"

# ==============================================================================
# DESENVOLVIMENTO E DEBUG
# ==============================================================================

# Compila versão debug
debug: CXXFLAGS += -g -DDEBUG -O0
debug: clean all
	@echo "🐛 Versão debug compilada"
	@echo "🔍 Use: gdb ./$(TCP_SERVER) ou gdb ./$(TCP_CLIENT)"

# Verifica arquivos e estrutura necessários
check: setup
	@echo "🔍 Verificando estrutura do projeto..."
	@echo "📁 Diretórios:"
	@for dir in $(SRC_DIR) $(LIB_DIR) $(OBJ_DIR) $(LOG_DIR); do \
		if [ -d $$dir ]; then echo "✅ $$dir/"; else echo "❌ $$dir/ (faltando)"; fi; \
	done
	@echo "📄 Headers:"
	@for file in $(HEADERS); do \
		if [ -f $$file ]; then echo "✅ $$file"; else echo "❌ $$file (faltando)"; fi; \
	done
	@echo "📄 Arquivos fonte esperados:"
	@for file in libtslog.cpp test_libtslog.cpp tcp_server.cpp tcp_client.cpp; do \
		if [ -f $(SRC_DIR)/$$file ]; then echo "✅ $(SRC_DIR)/$$file"; else echo "⚠️  $(SRC_DIR)/$$file (criar)"; fi; \
	done

# Comando para debuggar logs
debug-logs: setup
	@echo "🔍 DEBUG - Analisando logs detalhadamente..."
	@if [ -f $(SERVER_LOG) ]; then \
		echo "📄 Conteúdo do $(SERVER_LOG):"; \
		echo "====================================="; \
		cat $(SERVER_LOG); \
		echo "====================================="; \
		echo "📊 Análise de padrões:"; \
		echo "   Linhas totais: $$(wc -l < $(SERVER_LOG))"; \
		echo "   'conectado': $$(grep -c 'conectado' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
		echo "   'Mensagem recebida': $$(grep -c 'Mensagem recebida' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
		echo "   'Mensagem retransmitida': $$(grep -c 'Mensagem retransmitida' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
		echo "   'desconectado': $$(grep -c 'desconectado' $(SERVER_LOG) 2>/dev/null || echo 0)"; \
	else \
		echo "❌ Arquivo $(SERVER_LOG) não encontrado"; \
	fi

# ==============================================================================
# INFORMAÇÕES E AJUDA
# ==============================================================================

# Exibe informações do projeto
info: setup
	@echo "📋 INFORMAÇÕES DO PROJETO"
	@echo "===================================="
	@echo "Estrutura de diretórios:"
	@echo "  $(SRC_DIR)/     - Código fonte"
	@echo "  $(LIB_DIR)/     - Headers (.h)"
	@echo "  $(OBJ_DIR)/     - Objetos compilados"
	@echo "  $(LOG_DIR)/     - Arquivos de log"
	@echo ""
	@echo "Executáveis:"
	@echo "  $(TEST_LIBTSLOG) - Teste libtslog → $(TEST_LOG)"
	@echo "  $(TCP_SERVER)    - Servidor TCP → $(SERVER_LOG)"
	@echo "  $(TCP_CLIENT)    - Cliente CLI → $(CLIENT_LOG)"
	@echo ""
	@echo "Compilador: $(CXX) $(CXXFLAGS)"

# Ajuda completa
help:
	@echo "🚀 MAKEFILE - SERVIDOR DE CHAT TCP"
	@echo "============================================="
	@echo ""
	@echo "📦 COMPILAÇÃO:"
	@echo "  all              	- Compila todos os executáveis"
	@echo "  setup            	- Cria diretórios necessários"
	@echo ""
	@echo "▶️  EXECUÇÃO:"
	@echo "  run-test         	- Executa teste da libtslog"
	@echo "  run-server      	 - Inicia servidor TCP (porta 8080)"
	@echo "  run-client      	 - Inicia cliente TCP"
	@echo "  run-client-custom	- Inicia cliente TCP customizado"
	@echo ""
	@echo "🧪 TESTES:"
	@echo "  test-tcp       	  - Teste automatizado completo"
	@echo "  stress-test    	  - Teste de stress"
	@echo ""
	@echo "📊 LOGS:"
	@echo "  logs-summary    	 - Resumo de todos os logs"
	@echo "  logs-tail     	   - Monitora logs em tempo real"
	@echo "  debug-logs     	  - Análise detalhada dos logs"
	@echo "  clean-logs     	  - Remove apenas logs"
	@echo ""
	@echo "🧹 LIMPEZA:"
	@echo "  clean         	  	- Remove executáveis e logs"
	@echo "  clean-obj        	- Remove apenas objetos"
	@echo "  clean-all       	 - Limpeza total (remove pasta logs)"
	@echo ""
	@echo "📁 TODOS OS LOGS FICAM EM: $(LOG_DIR)/"
	@echo "   $(TEST_LOG)    - Teste da biblioteca"
	@echo "   $(SERVER_LOG)  - Servidor TCP"
	@echo "   $(CLIENT_LOG)  - Cliente TCP"

# ==============================================================================
# REGRAS ESPECIAIS
# ==============================================================================
.PHONY: all setup clean clean-obj clean-logs clean-all run-test run-server run-client run-client-custom test-tcp stress-test logs-summary logs-tail debug-logs debug check info help

# Não remove objetos intermediários automaticamente
.SECONDARY: $(LIBTSLOG_OBJ) $(TEST_LIBTSLOG_OBJ) $(TCP_SERVER_OBJ) $(TCP_CLIENT_OBJ)