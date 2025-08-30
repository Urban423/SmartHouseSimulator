#pragma once

inline int InitEngine() { return 0; }// Инициализация движка
inline int ShutdownEngine() { return 0; }  // Завершение работы движка
inline int LoadScene(const char* name) { return 0; }  // Загрузка сцены
inline int SaveScene(const char* name) { return 0; }  // Сохранение сцены
inline int RenderFrame() { return 0; } // Рендер кадра
inline int UpdateLogic() { return 0; } // Обновление логики
inline int ProcessInput() { return 0; }// Обработка ввода
inline int LoadTexture(const char* path) { return 0; }// Загрузка текстуры
inline int LoadModel(const char* path) { return 0; }  // Загрузка модели
inline int SpawnEntity(const char* type) { return 0; }// Создание объекта
inline int DestroyEntity(int id) { return 0; }  // Удаление объекта
inline int SetCameraPosition(float x, float y, float z) { return 0; } // Камера
inline int GetFPS() { return 0; }// Получение FPS
inline int SetResolution(int width, int height) { return 0; } // Установка разрешения
inline int ToggleFullscreen() { return 0; }		 // Переключение полноэкранного режима
inline int ReloadShaders() { return 0; }			// Перезагрузка шейдеров
inline int BuildNavigationMesh() { return 0; }                   // Построить навмеш
inline int FindPath(int entityId, float toX, float toY, float toZ) { return 0; } // Найти путь
inline int MoveEntityAlongPath(int entityId) { return 0; }       // Двигать по пути
inline int ClearNavigationData() { return 0; }                   // Очистить данные навигации