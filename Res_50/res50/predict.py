import torch
import torchvision.transforms as transforms
from PIL import Image
from torchvision.models import resnet50
import torch.nn as nn

CIFAR10_CLASSES = {
    0: 'airplane',
    1: 'automobile',
    2: 'bird',
    3: 'cat',
    4: 'deer',
    5: 'dog',
    6: 'frog',
    7: 'horse',
    8: 'ship',
    9: 'truck'
}

def predict_image(image_path, model_path, num_classes):
    # Load and preprocess image
    transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406],
                           std=[0.229, 0.224, 0.225])
    ])
    
    image = Image.open(image_path).convert('RGB')
    image = transform(image).unsqueeze(0)
    
    # Load model
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model = resnet50(weights=None)
    model.fc = nn.Linear(model.fc.in_features, num_classes)
    model.load_state_dict(torch.load(model_path))
    model = model.to(device)
    model.eval()
    
    # Predict
    with torch.no_grad():
        image = image.to(device)
        outputs = model(image)
        _, predicted = torch.max(outputs, 1)
        
    return predicted.item()

if __name__ == '__main__':
    # Example usage
    image_path = 'test_img/tst_image.png'
    model_path = 'models/saved_model.pth'
    num_classes = 10  # Replace with your number of classes
    prediction = predict_image(image_path, model_path, num_classes)
    class_name = CIFAR10_CLASSES[prediction]
    print(f'Predicted class: {prediction} ({class_name})')