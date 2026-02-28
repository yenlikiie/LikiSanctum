# LikiSanctum YOLO Project

YOLO-based computer vision project with two entry points:

- `main.py`: real-time webcam inference/tracking using a trained model (`best.pt`).
- `train.py`: model training using dataset config from `data.yaml` and base weights `yolo26n.pt`.

## Project Files

- `main.py` - webcam detection/tracking script.
- `train.py` - training script.
- `best.pt` - trained weights used by `main.py`.
- `data.yaml` - dataset paths and class metadata.

## Requirements

- Windows + Python 3.12 (or compatible Python 3.10+).
- Webcam for `main.py` live inference.

## Setup

```powershell
python -m venv .venv
.\.venv\Scripts\activate
python -m pip install --upgrade pip
pip install -r requirements.txt
```

## Run Inference (`main.py`)

```powershell
.\.venv\Scripts\activate
python main.py
```

- The script auto-selects device: CUDA if available, otherwise CPU.
- Press `Esc` to close the camera window.

## Train Model (`train.py`)

```powershell
.\.venv\Scripts\activate
python train.py
```

Training uses settings defined in `train.py` and dataset locations from `data.yaml`.
Make sure dataset folders referenced by `data.yaml` exist.
