
from ultralytics import YOLO

def main():
    model = YOLO("yolo26n.pt")
    model.train(
        data="data.yaml",  # путь к dataset.yaml
        epochs=50,  # количество эпох(кругов) обучения
        imgsz=640,  # размер картинок
        batch=16,  # количество картинок за один шаг
        workers=4,  # число параллельных процессов,
        project="likisan_train",  # папка для сохранения результатов
        name="exp1",  # название эксперимента
        exist_ok=True  # переписать если есть такая папка

    )









if __name__ == '__main__':
    main()
