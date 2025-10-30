# Compilateur et flags
CXX = g++
CXXFLAGS = 

# Noms des exécutables (avec .exe pour Windows)
CIPHER = cipher.exe
COMPUTER = computer.exe
STUB = stub.exe
PACKING = packing.exe
COMPRESSOR = compressor.exe

# Cible pour compiler et exécuter tous les tests
tests: $(CIPHER) $(COMPUTER) $(STUB) $(PACKING) $(COMPRESSOR)
	@echo ========================================
	@echo Execution des tests...
	@echo ========================================
	@echo.
	@echo === Test Cipher ===
	@$(CIPHER)
	@echo.
	@echo === Test Computer Name ===
	@$(COMPUTER)
	@echo.
	@echo === Test Stub ===
	@$(STUB)
	@echo.
	@echo === Test Packing ===
	@$(PACKING)
	@echo.
	@echo === Test Compressor ===
	@$(COMPRESSOR)
	@echo.
	@echo ========================================
	@echo Tous les tests sont termines
	@echo ========================================

# Compilation de chaque exécutable
$(CIPHER):
	$(CXX) $(CXXFLAGS) -o $(CIPHER) packer/Cipher.cpp tests/TestingTools.cpp tests/test_cipher.cpp

$(COMPUTER):
	$(CXX) $(CXXFLAGS) -o $(COMPUTER) packer/ComputerName.cpp tests/TestingTools.cpp tests/test_computer_name.cpp

$(STUB):
	$(CXX) $(CXXFLAGS) -o $(STUB) packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp tests/TestingTools.cpp tests/test_stub.cpp

$(PACKING):
	$(CXX) $(CXXFLAGS) -o $(PACKING) packer/Cipher.cpp packer/Stub.cpp packer/Compressor.cpp packer/Packer.cpp tests/TestingTools.cpp tests/test_packing_mechanism.cpp

$(COMPRESSOR):
	$(CXX) $(CXXFLAGS) -o $(COMPRESSOR) packer/Compressor.cpp tests/TestingTools.cpp tests/test_compressor.cpp

# Nettoyage
clean:
	@if exist $(CIPHER) del $(CIPHER)
	@if exist $(COMPUTER) del $(COMPUTER)
	@if exist $(STUB) del $(STUB)
	@if exist $(PACKING) del $(PACKING)
	@if exist $(COMPRESSOR) del $(COMPRESSOR)

.PHONY: tests clean