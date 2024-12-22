import torch
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms
from torch.utils.data import DataLoader
from torchvision.datasets import ImageFolder
from torchvision.models import resnet50, ResNet50_Weights
from tqdm import tqdm
import time

def train_model():
    print("初始化训练...")
    # Set device
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    print(f"使用设备: {device}")
    
    # Data transforms
    print("配置数据转换...")
    transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406],
                           std=[0.229, 0.224, 0.225])
    ])
    
    # Load datasets
    print("加载训练数据集...")
    train_dataset = ImageFolder('data/train', transform=transform)
    print(f"训练数据集类别数: {len(train_dataset.classes)}")
    print(f"训练样本总数: {len(train_dataset)}")
    
    print("加载验证数据集...")
    val_dataset = ImageFolder('data/val', transform=transform)
    print(f"验证样本总数: {len(val_dataset)}")
    
    print("创建数据加载器...")
    train_loader = DataLoader(train_dataset, batch_size=32, shuffle=True)
    val_loader = DataLoader(val_dataset, batch_size=32, shuffle=False)
    
    # Load model
    print("加载ResNet50模型...")
    model = resnet50(weights=ResNet50_Weights.DEFAULT)
    num_classes = len(train_dataset.classes)
    model.fc = nn.Linear(model.fc.in_features, num_classes)
    model = model.to(device)
    
    # Loss and optimizer
    criterion = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
    
    # Training loop
    num_epochs = 10
    print("\n开始训练...")
    for epoch in range(num_epochs):
        model.train()
        running_loss = 0.0
        
        # 创建训练进度条
        train_pbar = tqdm(train_loader, desc=f'Epoch {epoch+1}/{num_epochs} [Train]')
        for i, (images, labels) in enumerate(train_pbar):
            images = images.to(device)
            labels = labels.to(device)
            
            outputs = model(images)
            loss = criterion(outputs, labels)
            
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            
            running_loss += loss.item()
            train_pbar.set_postfix({'loss': f'{running_loss/(i+1):.4f}'})
        
        # Validation
        model.eval()
        correct = 0
        total = 0
        val_loss = 0.0
        
        # 创建验证进度条
        val_pbar = tqdm(val_loader, desc=f'Epoch {epoch+1}/{num_epochs} [Val]')
        with torch.no_grad():
            for i, (images, labels) in enumerate(val_pbar):
                images = images.to(device)
                labels = labels.to(device)
                outputs = model(images)
                loss = criterion(outputs, labels)
                val_loss += loss.item()
                
                _, predicted = torch.max(outputs.data, 1)
                total += labels.size(0)
                correct += (predicted == labels).sum().item()
                
                val_pbar.set_postfix({
                    'loss': f'{val_loss/(i+1):.4f}',
                    'acc': f'{100 * correct / total:.2f}%'
                })
        
        print(f'Epoch [{epoch+1}/{num_epochs}]')
        print(f'Training Loss: {running_loss/len(train_loader):.4f}')
        print(f'Validation Loss: {val_loss/len(val_loader):.4f}')
        print(f'Validation Accuracy: {100 * correct / total:.2f}%\n')
    
    print("保存模型...")
    torch.save(model.state_dict(), 'models/saved_model.pth')
    print("训练完成！")
    
if __name__ == '__main__':
    start_time = time.time()
    train_model()
    end_time = time.time()
    print(f"总训练时间: {(end_time - start_time)/60:.2f} 分钟")